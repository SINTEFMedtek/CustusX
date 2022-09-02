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
#include "cxDataSelectWidget.h"

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

	layout->addWidget(new DataSelectWidget(viewService, patientModelService, this, StringPropertyActiveImage::New(patientModelService)));
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	layout->addLayout(buttonLayout);
	QHBoxLayout* buttonLayout2 = new QHBoxLayout;
	layout->addLayout(buttonLayout2);

	mShowEraserCheckBox = new QCheckBox("Show eraser");
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
	mSphereSizeAdapter = DoubleProperty::initialize("SphereSize", "Sphere Radius (mm)", "Radius of Eraser Sphere in mm", sphereRadius, DoubleRange(0,100,0.1), 1, QDomNode());
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

	if (!original)
		return;

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
	ImagePtr image = mActiveData->getActive<Image>();
	if (!image)
		return;

	mPatientModelService->insertData(image);
}


template <class TYPE>
void EraserWidget::eraseVolume(TYPE* volumePointer)
{
	ImagePtr image = mActiveData->getActive<Image>();
	vtkImageDataPtr img = image->getBaseVtkImageData();
	Eigen::Array3i dim(img->GetDimensions());
	Eigen::Array3d spacing(img->GetSpacing());
	Transform3D rMd = mViewService->getGroup(0)->getOptions().mPickerGlyph->get_rMd();
	Eigen::Array3d c(mSphere->GetCenter());
	c = rMd.coord(c);
	double r = mSphere->GetRadius();
	int replaceVal = mEraseValueAdapter->getValue();

	mPreviousCenter = c;
	mPreviousRadius = r;

	Eigen::Array3d scaledRadius = r*spacing.inverse();
	Transform3D dMr = image->get_rMd().inv();
	Transform3D rawMd = createTransformScale(spacing).inv();
	Transform3D rawMr = rawMd * dMr;
	c = rawMr.coord(c); //Center voxel

	Eigen::Array3i lowVoxIdx = (c - scaledRadius).floor().cast<int>();
	lowVoxIdx = lowVoxIdx.max(0);
	Eigen::Array3i highVoxIdx = (c + scaledRadius).ceil().cast<int>();
	highVoxIdx = highVoxIdx.min(dim);

	for (int x = lowVoxIdx(0); x < highVoxIdx(0); ++x)
		for (int y = lowVoxIdx(1); y < highVoxIdx(1); ++y)
			for (int z = lowVoxIdx(2); z < highVoxIdx(2); ++z)
			{
				int index = x + y * dim[0] + z * dim[0] * dim[1];
				if ((Vector3D((x-c(0))*spacing[0], (y-c(1))*spacing[1], (z-c(2))*spacing[2])).length() < r)
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
	if (!image)
		return;

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
