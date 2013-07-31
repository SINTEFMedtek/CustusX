// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include <cxEraserWidget.h>

#include <QTimer>

#include "vtkSphereWidget.h"
#include "vtkSplineWidget.h"
#include "vtkSplineWidget2.h"
#include "vtkSplineRepresentation.h"
#include "vtkRenderWindow.h"
#include <vtkSphere.h>
#include <vtkClipPolyData.h>
#include <vtkImageData.h>

#include "sscMesh.h"
#include "sscStringDataAdapter.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscDefinitionStrings.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscMessageManager.h"
#include "sscRegistrationTransform.h"
#include "sscImageAlgorithms.h"
#include "sscDoubleWidgets.h"
#include "sscImage.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxView3D.h"
#include "cxInteractiveCropper.h"
#include "cxViewManager.h"
#include "cxToolManager.h"
#include "cxViewWrapper.h"
#include "cxViewGroup.h"

namespace cx
{

EraserWidget::EraserWidget(QWidget* parent) :
	BaseWidget(parent, "EraserWidget", "Eraser"),
	mPreviousCenter(0,0,0),
	mPreviousRadius(0)
{

	QVBoxLayout* layout = new QVBoxLayout(this);

	mContinousEraseTimer = new QTimer(this);
	connect(mContinousEraseTimer, SIGNAL(timeout()), this, SLOT(continousRemoveSlot())); // this signal will be executed in the thread of THIS, i.e. the main thread.

//	this->setToolTip(this->defaultWhatsThis());

//	layout->addWidget(new QLabel(QString("<font size=4 color=red><b>%1</b></font><br>Erase parts of active volume using a sphere.").arg("Experimental Widget!!")));
//	layout->addStretch();

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
	mSphereSizeAdapter = ssc::DoubleDataAdapterXml::initialize("SphereSize", "Sphere Size", "Radius of Eraser Sphere", sphereRadius, ssc::DoubleRange(1,200,1), 0, QDomNode());
	connect(mSphereSizeAdapter.get(), SIGNAL(changed()), this, SLOT(sphereSizeChangedSlot()));
	mSphereSize = new ssc::SpinBoxAndSliderGroupWidget(this, mSphereSizeAdapter);
	layout->addWidget(mSphereSize);

	layout->addStretch();

	this->enableButtons();
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

QString EraserWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Functionality for erasing parts of volumes/meshes.</h3>"
		"<p>"
		"</p>"
		"<p><i></i></p>"
		"</html>";
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
	ssc::Transform3D rMd = viewManager()->getViewGroups().front()->getData()->getOptions().mPickerGlyph->get_rMd();
	ssc::Vector3D c(mSphere->GetCenter());
	c = rMd.coord(c);
	double r = mSphere->GetRadius();

	// optimization: dont remove if idle
	if (ssc::similar(mPreviousCenter, c) && ssc::similar(mPreviousRadius, r))
		return;

	this->removeSlot();
}

void EraserWidget::duplicateSlot()
{
	ssc::ImagePtr original = ssc::dataManager()->getActiveImage();
	QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();

	ssc::ImagePtr duplicate = duplicateImage(original);
	ssc::dataManager()->loadData(duplicate);
	ssc::dataManager()->saveImage(duplicate, outputBasePath);
	ssc::dataManager()->setActiveImage(duplicate);

	// replace viz of original with duplicate
	std::vector<ViewGroupPtr> viewGroups = viewManager()->getViewGroups();
	for (unsigned i = 0; i < viewGroups.size(); ++i)
	{
		if (viewGroups[i]->getData()->removeData(original))
			viewGroups[i]->getData()->addData(duplicate);
	}
}

void EraserWidget::sphereSizeChangedSlot()
{
	if (mSphere)
		mSphere->SetRadius(mSphereSizeAdapter->getValue());
}

/**The image data themselves are not saved during normal file save.
 * This slot saves the file data to mhd.
 *
 */
void EraserWidget::saveSlot()
{
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();

	ssc::dataManager()->saveImage(image, outputBasePath);
}


template <class TYPE>
void EraserWidget::eraseVolume(TYPE* volumePointer, TYPE replaceVal)
{
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	vtkImageDataPtr img = image->getBaseVtkImageData();

//	std::cout << "starting" << std::endl;

	Eigen::Array3i dim(img->GetDimensions());
	ssc::Vector3D spacing(img->GetSpacing());

	ssc::Transform3D rMd = viewManager()->getViewGroups().front()->getData()->getOptions().mPickerGlyph->get_rMd();
	ssc::Vector3D c(mSphere->GetCenter());
	c = rMd.coord(c);
	double r = mSphere->GetRadius();
	mPreviousCenter = c;
	mPreviousRadius = r;

	ssc::DoubleBoundingBox3D bb_r(c[0]-r, c[0]+r, c[1]-r, c[1]+r, c[2]-r, c[2]+r);

	ssc::Transform3D dMr = image->get_rMd().inv();
	ssc::Transform3D rawMd = ssc::createTransformScale(spacing).inv();
	ssc::Transform3D rawMr = rawMd * dMr;
	ssc::Vector3D c_d = dMr.coord(c);
	double r_d = dMr.vector(r * ssc::Vector3D::UnitX()).length();
	c = rawMr.coord(c);
	r = rawMr.vector(r * ssc::Vector3D::UnitX()).length();
	ssc::DoubleBoundingBox3D bb0_raw = ssc::transform(rawMr, bb_r);
//	ssc::IntBoundingBox3D bb1_raw(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
	ssc::IntBoundingBox3D bb1_raw(0, dim[0], 0, dim[1], 0, dim[2]);

//	std::cout << "     sphere: " << bb0_raw << std::endl;
//	std::cout << "        raw: " << bb1_raw << std::endl;

	for (int i=0; i<3; ++i)
	{
		bb1_raw[2*i] = std::max<double>(bb1_raw[2*i], bb0_raw[2*i]);
		bb1_raw[2*i+1] = std::min<double>(bb1_raw[2*i+1], bb0_raw[2*i+1]);
	}

//	std::cout << "clip in raw: " << bb1_raw << std::endl;
	//	double r=50;
	//	ssc::Vector3D c(200,200,200);
	for (int x = bb1_raw[0]; x < bb1_raw[1]; ++x)
		for (int y = bb1_raw[2]; y < bb1_raw[3]; ++y)
			for (int z = bb1_raw[4]; z < bb1_raw[5]; ++z)
			{
				int index = x + y * dim[0] + z * dim[0] * dim[1];
//				volumePointer[index] = replaceVal;

//				if ((ssc::Vector3D(x, y, z) - c).length() < r)
//					volumePointer[index] = replaceVal;
				if ((ssc::Vector3D(x*spacing[0], y*spacing[1], z*spacing[2]) - c_d).length() < r_d)
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

//	vtkPolyDataPtr poly = vtkPolyDataPtr::New();
//	mEraserSphere->GetPolyData(poly);

#if 0
	// experimental clipping of mesh - has no effect...
	std::map<QString,ssc::MeshPtr> meshes = ssc::dataManager()->getMeshes();
	if (!meshes.empty())
	{
		ssc::MeshPtr mesh = meshes.begin()->second;

		ssc::Vector3D c(mEraserSphere->GetCenter());
		double r = mEraserSphere->GetRadius();
		ssc::Transform3D dMr = mesh->get_rMd().inv();
		ssc::Vector3D c_d = dMr.coord(c);
		double r_d = dMr.vector(r * ssc::Vector3D::UnitX()).length();
		vtkSphere* sphere = vtkSphere::New();
		sphere->SetRadius(r_d);
		sphere->SetCenter(c_d.data());


//		mEraserSphere->GetSphere(sphere);
		vtkClipPolyData* clipper = vtkClipPolyData::New();
		clipper->SetInput(mesh->getVtkPolyData());
		clipper->SetClipFunction(sphere);
		clipper->Update();
		mesh->setVtkPolyData(clipper->GetOutput());
		return;
	}
#endif

	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	vtkImageDataPtr img = image->getBaseVtkImageData();

	if (img->GetScalarType()==VTK_CHAR)
		this->eraseVolume(static_cast<char*> (img->GetScalarPointer()), VTK_CHAR_MIN);
	if (img->GetScalarType()==VTK_UNSIGNED_CHAR)
		this->eraseVolume(static_cast<unsigned char*> (img->GetScalarPointer()), VTK_UNSIGNED_CHAR_MIN);
	if (img->GetScalarType()==VTK_UNSIGNED_SHORT)
		this->eraseVolume(static_cast<unsigned short*> (img->GetScalarPointer()), VTK_UNSIGNED_SHORT_MIN);
	if (img->GetScalarType()==VTK_SHORT)
		this->eraseVolume(static_cast<short*> (img->GetScalarPointer()), VTK_SHORT_MIN);
	if (img->GetScalarType()==VTK_UNSIGNED_INT)
		this->eraseVolume(static_cast<unsigned int*> (img->GetScalarPointer()), VTK_UNSIGNED_INT_MIN);
	if (img->GetScalarType()==VTK_INT)
		this->eraseVolume(static_cast<int*> (img->GetScalarPointer()), VTK_INT_MIN);

	ssc::ImageLUT2DPtr tf2D = image->getLookupTable2D();
	ssc::ImageTF3DPtr tf3D = image->getTransferFunctions3D();

	img->Modified();
	image->setVtkImageData(img);

	// keep existing transfer functions
	image->setLookupTable2D(tf2D);
	image->setTransferFunctions3D(tf3D);
}

void EraserWidget::toggleShowEraser(bool on)
{
	if (on)
	{
		std::vector<ViewGroupPtr> viewGroups = viewManager()->getViewGroups();
		mSphere = vtkSphereSourcePtr::New();

		mSphere->SetRadius(40);
		mSphere->SetThetaResolution(16);
		mSphere->SetPhiResolution(12);
		mSphere->LatLongTessellationOn(); // more natural wireframe view

		double a = mSphereSizeAdapter->getValue();
		mSphere->SetRadius(a);
		ssc::MeshPtr glyph = viewGroups.front()->getData()->getOptions().mPickerGlyph;
		glyph->setVtkPolyData(mSphere->GetOutput());
		glyph->setColor(QColor(255, 204, 0)); // same as tool
		glyph->setIsWireframe(true);

		// set same glyph in all groups
		for (unsigned i=0; i<viewGroups.size(); ++i)
		{
			ViewGroupData::Options options = viewGroups[i]->getData()->getOptions();
			options.mPickerGlyph = glyph;
			viewGroups[i]->getData()->setOptions(options);
		}
	}
	else
	{
		viewManager()->getViewGroups().front()->getData()->getOptions().mPickerGlyph->setVtkPolyData(NULL);
		mContinousEraseCheckBox->setChecked(false);
	}

	this->enableButtons();
}

}
