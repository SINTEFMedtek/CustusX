/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxEraserWidget.h>

#include <QTimer>
#include <QCheckBox>

#include "vtkSphereWidget.h"
#include "vtkSplineWidget.h"
#include "vtkSplineWidget2.h"
#include "vtkSplineRepresentation.h"
#include "vtkRenderWindow.h"
#include <vtkSphere.h>
#include <vtkClipPolyData.h>
#include <vtkImageData.h>

#include "cxMesh.h"
#include "cxStringPropertyBase.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDefinitionStrings.h"
#include "cxUtilHelpers.h"

#include "cxRegistrationTransform.h"
#include "cxImageAlgorithms.h"
#include "cxDoubleWidgets.h"
#include "cxImage.h"
#include "cxVolumeHelpers.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxReporter.h"
#include "cxActiveData.h"

namespace cx
{

EraserWidget::EraserWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent) :
	BaseWidget(parent, "eraser_widget", "Eraser"),
	mPreviousCenter(0,0,0),
	mPreviousRadius(0),
	mActiveImageProxy(ActiveImageProxyPtr()),
	mPatientModelService(patientModelService),
	mViewService(viewService),
	mActiveData(patientModelService->getActiveData())
{

	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setToolTip("Erase parts of volumes/models");

	mContinousEraseTimer = new QTimer(this);
	connect(mContinousEraseTimer, SIGNAL(timeout()), this, SLOT(continousRemoveSlot())); // this signal will be executed in the thread of THIS, i.e. the main thread.

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	layout->addLayout(buttonLayout);
	QHBoxLayout* buttonLayout2 = new QHBoxLayout;
	layout->addLayout(buttonLayout2);

	mShowEraserCheckBox = new QCheckBox("Show");
	mShowEraserCheckBox->setToolTip("Show eraser sphere in the views.");
	connect(mShowEraserCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleShowEraser(bool)));
	buttonLayout->addWidget(mShowEraserCheckBox);

	mContinousEraseCheckBox = new QCheckBox("Continous");
	mContinousEraseCheckBox->setToolTip("Erase continously using the sphere. (might be slow)");
	connect(mContinousEraseCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleContinous(bool)));
	buttonLayout2->addWidget(mContinousEraseCheckBox);

	mDuplicateAction = this->createAction(this, QIcon(), "Duplicate", "Duplicate active volume - do this before erasing!",
		SLOT(duplicateSlot()), buttonLayout);

	mSaveAction = this->createAction(this, QIcon(), "Save", "Save modified image to disk",
		SLOT(saveSlot()), buttonLayout);

	mRemoveAction = this->createAction(this, QIcon(), "Erase", "Erase everything inside sphere",
		SLOT(removeSlot()), buttonLayout2);


	double sphereRadius = 10;
	mSphereSizeAdapter = DoubleProperty::initialize("SphereSize", "Sphere Size", "Radius of Eraser Sphere", sphereRadius, DoubleRange(0,200,1), 0, QDomNode());
	connect(mSphereSizeAdapter.get(), &DoubleProperty::changed, this, &EraserWidget::sphereSizeChangedSlot);
	mSphereSize = new SpinBoxAndSliderGroupWidget(this, mSphereSizeAdapter);
	layout->addWidget(mSphereSize);

	ImagePtr image = mActiveData->getActive<Image>();
	int eraseValue = 0;
	if(image)
		eraseValue = image->getMin();
	mEraseValueAdapter = DoubleProperty::initialize("EraseValue", "Erase value", "Erase/draw with value", eraseValue, DoubleRange(1,200,1), 0, QDomNode());

	mActiveImageProxy = ActiveImageProxy::New(mActiveData);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &EraserWidget::activeImageChangedSlot);

	mEraseValueWidget = new SpinBoxAndSliderGroupWidget(this, mEraseValueAdapter);
	layout->addWidget(mEraseValueWidget);

	layout->addStretch();

	this->enableButtons();
}

void EraserWidget::activeImageChangedSlot()
{
	ImagePtr image = mActiveData->getActive<Image>();
	if(!image)
		return;

	mEraseValueAdapter->setValueRange(DoubleRange(image->getVTKMinValue(), image->getVTKMaxValue(), 1));
	mEraseValueAdapter->setValue(image->getMin());
}

void EraserWidget::enableButtons()
{
	bool e = mShowEraserCheckBox->isChecked();

	mContinousEraseCheckBox->setEnabled(e);
//	mDuplicateAction->setEnabled(e);
//	mSaveAction->setEnabled(e);
	mRemoveAction->setEnabled(e);
	mSphereSize->setEnabled(e);
}

EraserWidget::~EraserWidget()
{
}

void EraserWidget::toggleContinous(bool on)
{
	if (on)
	{
		mContinousEraseTimer->start(300);
	}
	else
	{
		mContinousEraseTimer->stop();
	}
}

void EraserWidget::continousRemoveSlot()
{
	Transform3D rMd = mViewService->getGroup(0)->getOptions().mPickerGlyph->get_rMd();
	//Transform3D rMd = mViewService->getViewGroupDatas().front()->getData()->getOptions().mPickerGlyph->get_rMd();
	Vector3D c(mSphere->GetCenter());
	c = rMd.coord(c);
	double r = mSphere->GetRadius();

	// optimization: dont remove if idle
	if (similar(mPreviousCenter, c) && similar(mPreviousRadius, r))
		return;

	this->removeSlot();
}

void EraserWidget::duplicateSlot()
{
	ImagePtr original = mActiveData->getActive<Image>();

	ImagePtr duplicate = duplicateImage(mPatientModelService, original);
	mPatientModelService->insertData(duplicate);
	mActiveData->setActive(duplicate);

	// replace viz of original with duplicate
//	std::vector<ViewGroupPtr> viewGroups = mViewService->getViewGroupDatas();
	for (unsigned i = 0; i < mViewService->groupCount(); ++i)
	{
		if (mViewService->getGroup(i)->removeData(original->getUid()))
			mViewService->getGroup(i)->addData(duplicate->getUid());
	}
}

void EraserWidget::sphereSizeChangedSlot()
{
	if (mSphere)
	{
		mSphere->SetRadius(mSphereSizeAdapter->getValue());
		mSphere->Update();
	}
}

/**The image data themselves are not saved during normal file save.
 * This slot saves the file data to mhd.
 *
 */
void EraserWidget::saveSlot()
{
	mPatientModelService->insertData(mActiveData->getActive<Image>());
}


template <class TYPE>
void EraserWidget::eraseVolume(TYPE* volumePointer)
{
	ImagePtr image = mActiveData->getActive<Image>();
	vtkImageDataPtr img = image->getBaseVtkImageData();


	Eigen::Array3i dim(img->GetDimensions());
	Vector3D spacing(img->GetSpacing());

	Transform3D rMd = mViewService->getGroup(0)->getOptions().mPickerGlyph->get_rMd();
	Vector3D c(mSphere->GetCenter());
	c = rMd.coord(c);
	double r = mSphere->GetRadius();
	mPreviousCenter = c;
	mPreviousRadius = r;

	DoubleBoundingBox3D bb_r(c[0]-r, c[0]+r, c[1]-r, c[1]+r, c[2]-r, c[2]+r);

	Transform3D dMr = image->get_rMd().inv();
	Transform3D rawMd = createTransformScale(spacing).inv();
	Transform3D rawMr = rawMd * dMr;
	Vector3D c_d = dMr.coord(c);
	double r_d = dMr.vector(r * Vector3D::UnitX()).length();
	c = rawMr.coord(c);
	r = rawMr.vector(r * Vector3D::UnitX()).length();
	DoubleBoundingBox3D bb0_raw = transform(rawMr, bb_r);
	IntBoundingBox3D bb1_raw(0, dim[0], 0, dim[1], 0, dim[2]);

//	std::cout << "     sphere: " << bb0_raw << std::endl;
//	std::cout << "        raw: " << bb1_raw << std::endl;

	for (int i=0; i<3; ++i)
	{
		bb1_raw[2*i] = std::max<double>(bb1_raw[2*i], bb0_raw[2*i]);
		bb1_raw[2*i+1] = std::min<double>(bb1_raw[2*i+1], bb0_raw[2*i+1]);
	}

	int replaceVal = mEraseValueAdapter->getValue();

	for (int x = bb1_raw[0]; x < bb1_raw[1]; ++x)
		for (int y = bb1_raw[2]; y < bb1_raw[3]; ++y)
			for (int z = bb1_raw[4]; z < bb1_raw[5]; ++z)
			{
				int index = x + y * dim[0] + z * dim[0] * dim[1];
				if ((Vector3D(x*spacing[0], y*spacing[1], z*spacing[2]) - c_d).length() < r_d)
					volumePointer[index] = replaceVal;
			}
}

//#define VTK_VOID            0
//#define VTK_BIT             1
//#define VTK_CHAR            2
//#define VTK_SIGNED_CHAR    15
//#define VTK_UNSIGNED_CHAR   3
//#define VTK_SHORT           4
//#define VTK_UNSIGNED_SHORT  5
//#define VTK_INT             6
//#define VTK_UNSIGNED_INT    7
//#define VTK_LONG            8
//#define VTK_UNSIGNED_LONG   9
//#define VTK_FLOAT          10
//#define VTK_DOUBLE         11
//#define VTK_ID_TYPE        12

void EraserWidget::removeSlot()
{
	if (!mSphere)
		return;

	ImagePtr image = mActiveData->getActive<Image>();
	vtkImageDataPtr img = image->getBaseVtkImageData();

	int vtkScalarType = img->GetScalarType();

	if (vtkScalarType==VTK_CHAR)
		this->eraseVolume(static_cast<char*> (img->GetScalarPointer()));
	else if (vtkScalarType==VTK_UNSIGNED_CHAR)
		this->eraseVolume(static_cast<unsigned char*> (img->GetScalarPointer()));
	else if (vtkScalarType==VTK_SIGNED_CHAR)
		this->eraseVolume(static_cast<signed char*> (img->GetScalarPointer()));
	else if (vtkScalarType==VTK_UNSIGNED_SHORT)
		this->eraseVolume(static_cast<unsigned short*> (img->GetScalarPointer()));
	else if (vtkScalarType==VTK_SHORT)
		this->eraseVolume(static_cast<short*> (img->GetScalarPointer()));
	else if (vtkScalarType==VTK_UNSIGNED_INT)
		this->eraseVolume(static_cast<unsigned int*> (img->GetScalarPointer()));
	else if (vtkScalarType==VTK_INT)
		this->eraseVolume(static_cast<int*> (img->GetScalarPointer()));
	else
		reportError(QString("Unknown VTK ScalarType: %1").arg(vtkScalarType));

	ImageLUT2DPtr tf2D = image->getLookupTable2D();
	ImageTF3DPtr tf3D = image->getTransferFunctions3D();

//	img->Modified();
	setDeepModified(img);
	image->setVtkImageData(img);

	// keep existing transfer functions
	image->setLookupTable2D(tf2D);
	image->setTransferFunctions3D(tf3D);
}

void EraserWidget::toggleShowEraser(bool on)
{
	if (on)
	{
//		std::vector<ViewGroupPtr> viewGroups = mViewService->getViewGroups();
		mSphere = vtkSphereSourcePtr::New();

		mSphere->SetRadius(40);
		mSphere->SetThetaResolution(16);
		mSphere->SetPhiResolution(12);
		mSphere->LatLongTessellationOn(); // more natural wireframe view

		double a = mSphereSizeAdapter->getValue();
		mSphere->SetRadius(a);
		mSphere->Update();
		MeshPtr glyph = mViewService->getGroup(0)->getOptions().mPickerGlyph;
		glyph->setVtkPolyData(mSphere->GetOutput());
		glyph->setColor(QColor(255, 204, 0)); // same as tool
		glyph->setIsWireframe(true);

		// set same glyph in all groups
		for (unsigned i=0; i<mViewService->groupCount(); ++i)
		{
			ViewGroupData::Options options = mViewService->getGroup(i)->getOptions();
			options.mPickerGlyph = glyph;
			mViewService->getGroup(i)->setOptions(options);
		}
	}
	else
	{
		mViewService->getGroup(0)->getOptions().mPickerGlyph->setVtkPolyData(NULL);
		mContinousEraseCheckBox->setChecked(false);
	}

	this->enableButtons();
}

}
