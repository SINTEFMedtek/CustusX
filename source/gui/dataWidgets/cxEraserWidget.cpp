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

#include "vtkSphereWidget.h"
#include "vtkSplineWidget.h"
#include "vtkSplineWidget2.h"
#include "vtkSplineRepresentation.h"
#include "cxView3D.h"
#include "vtkRenderWindow.h"
#include <vtkSphere.h>
#include <vtkClipPolyData.h>
#include "sscMesh.h"

#include "sscStringDataAdapter.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscDefinitionStrings.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscMessageManager.h"
#include "sscRegistrationTransform.h"
#include "cxInteractiveCropper.h"
#include "cxPatientData.h"
#include <vtkImageData.h>
#include "sscImageAlgorithms.h"
#include "cxPatientService.h"
#include "cxViewManager.h"
#include "sscDoubleWidgets.h"
#include "cxToolManager.h"

namespace cx
{

EraserWidget::EraserWidget(QWidget* parent) :
	BaseWidget(parent, "EraserWidget", "Eraser")
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	this->setToolTip(this->defaultWhatsThis());

	layout->addWidget(new QLabel(QString("<font size=4 color=red><b>%1</b></font><br>Erase parts of active volume using a sphere.").arg("Experimental Widget!!")));
	layout->addStretch();

	mShowEraserCheckBox = new QCheckBox("Show eraser");
	mShowEraserCheckBox->setToolTip("Show eraser sphere in 3D view.");
	connect(mShowEraserCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleShowEraser(bool)));
	layout->addWidget(mShowEraserCheckBox);

	// QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png")
//	this->createAction(this, , "Test", "Test",
//		SLOT(testSlot()), layout);
	this->createAction(this, QIcon(), "Erase", "Erase everything inside sphere",
		SLOT(removeSlot()), layout);

	this->createAction(this, QIcon(), "Save", "Save modified image to disk",
		SLOT(saveSlot()), layout);

	double sphereRadius = 10;
	mSphereSizeAdapter = ssc::DoubleDataAdapterXml::initialize("SphereSize", "Sphere Size", "Radius of Eraser Sphere", sphereRadius, ssc::DoubleRange(1,200,1), 0, QDomNode());
	connect(mSphereSizeAdapter.get(), SIGNAL(changed()), this, SLOT(sphereSizeChangedSlot()));
	layout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mSphereSizeAdapter));
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

void EraserWidget::sphereSizeChangedSlot()
{
	if (mEraserSphere)
		mEraserSphere->SetRadius(mSphereSizeAdapter->getValue());
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

	std::cout << "starting" << std::endl;

	Eigen::Array3i dim(img->GetDimensions());
	ssc::Vector3D spacing(img->GetSpacing());

	ssc::Vector3D c(mEraserSphere->GetCenter());
	double r = mEraserSphere->GetRadius();

	ssc::DoubleBoundingBox3D bb_r(c[0]-r, c[0]+r, c[1]-r, c[1]+r, c[2]-r, c[2]+r);

	ssc::Transform3D dMr = image->get_rMd().inv();
	ssc::Transform3D rawMd = ssc::createTransformScale(spacing).inv();
	ssc::Transform3D rawMr = rawMd * dMr;
	ssc::Vector3D c_d = dMr.coord(c);
	double r_d = dMr.vector(r * ssc::Vector3D::UnitX()).length();
	c = rawMr.coord(c);
	r = rawMr.vector(r * ssc::Vector3D::UnitX()).length();
	ssc::DoubleBoundingBox3D bb0_raw = ssc::transform(rawMr, bb_r);
	ssc::IntBoundingBox3D bb1_raw(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);

	for (int i=0; i<3; ++i)
	{
		bb1_raw[2*i] = std::max<double>(bb1_raw[2*i], bb0_raw[2*i]);
		bb1_raw[2*i+1] = std::min<double>(bb1_raw[2*i+1], bb0_raw[2*i+1]);
	}

	std::cout << "clip in raw: " << bb1_raw << std::endl;
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

//	//	ssc::Vector3D c(200,200,200);
//	ssc::Vector3D c(mEraserSphere->GetCenter());
//	double r = mEraserSphere->GetRadius();
//
//	ssc::DoubleBoundingBox3D bb_r(c[0]-r, c[0]+r, c[1]-r, c[1]+r, c[2]-r, c[2]+r);
//
//	ssc::Transform3D dMr = image->get_rMd().inv();
//	ssc::Transform3D rawMd = ssc::createTransformScale(targetSpacing).inv();
//	ssc::Transform3D rawMr = rawMd * dMr;
//	c = rawMr.coord(c);
//	r = rawMr.vector(r * ssc::Vector3D::UnitX()).length();
//	ssc::DoubleBoundingBox3D bb0_raw = ssc::transform(rawMr, bb_r);
//	ssc::IntBoundingBox3D bb1_raw(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
//
//	for (int i=0; i<3; ++i)
//	{
//		bb1_raw[2*i] = std::max<double>(bb1_raw[2*i], bb0_raw[2*i]);
//		bb1_raw[2*i+1] = std::max<double>(bb1_raw[2*i+1], bb0_raw[2*i+1]);
//	}
//
//	std::cout << "clip in raw: " << bb1_raw << std::endl;
//	//	double r=50;
//	//	ssc::Vector3D c(200,200,200);
//	for (int x = bb1_raw[0]; x < bb1_raw[1]; ++x)
//		for (int y = bb1_raw[2]; y < bb1_raw[3]; ++y)
//			for (int z = bb1_raw[4]; z < bb1_raw[5]; ++z)
//			{
//				int index = x + y * dim[0] + z * dim[0] * dim[1];
//				outputPointer[index] = 255;
//
//				if ((ssc::Vector3D(x, y, z) - c).length() < r)
//					outputPointer[index] = 255;
//			}

//	for (int x = 0; x < dim[0]; ++x)
//		for (int y = 0; y < dim[1]; ++y)
//			for (int z = 0; z < dim[2]; ++z)
//			{
//				int index = x + y * dim[0] + z * dim[0] * dim[1];
//
//				if ((ssc::Vector3D(x, y, z) - c).length() < r)
//					outputPointer[index] = 255;
//			}
	image->setVtkImageData(img);
}

void EraserWidget::toggleShowEraser(bool on)
{
	ssc::View* view = viewManager()->get3DView();

	if (on)
	{
		mEraserSphere = vtkSphereWidgetPtr::New();
		mEraserSphere->SetInteractor(view->getRenderWindow()->GetInteractor());
		mEraserSphere->SetEnabled(true);
		double a = mSphereSizeAdapter->getValue();
		mEraserSphere->SetRadius(a);
		cx::ToolManager* tm = cx::ToolManager::getInstance();
		ssc::Transform3D rMt =
						*tm->get_rMpr() *
						tm->getManualTool()->get_prMt();
		mEraserSphere->SetCenter(rMt.coord(ssc::Vector3D(0, 0, tm->getTooltipOffset())).data());
//		mEraserSphere->PlaceWidget(-a, a, -a, a, -a, a);
		mEraserSphere->ScaleOn();

		mEraserSphere->SetThetaResolution(12);
		mEraserSphere->SetPhiResolution(12);
	}
	else
	{
		mEraserSphere->SetEnabled(false);
		mEraserSphere->SetInteractor(NULL);
		mEraserSphere = vtkSphereWidgetPtr();
	}
}

//void EraserWidget::testSlot()
//{
//	typedef vtkSmartPointer<vtkSplineWidget> vtkSplineWidgetPtr;
//	typedef vtkSmartPointer<vtkSplineWidget2> vtkSplineWidget2Ptr;
//	typedef vtkSmartPointer<vtkSplineRepresentation> vtkSplineRepresentationPtr;
//
//	ssc::View* view = viewManager()->get3DView();
//	mEraserSphere = vtkSphereWidget::New();
//	mEraserSphere->SetInteractor(view->getRenderWindow()->GetInteractor());
//	mEraserSphere->SetEnabled(true);
//	int a = 50;
//	mEraserSphere->PlaceWidget(-a, a, -a, a, -a, a);
//	mEraserSphere->ScaleOn();
//
//	mEraserSphere->SetThetaResolution(12);
//	mEraserSphere->SetPhiResolution(12);
//
//	////	vtkSplineWidget2Ptr node1 = vtkSplineWidget2Ptr::New();
//	//	vtkSplineWidget2* node1 = vtkSplineWidget2::New();
//	//	vtkSplineRepresentationPtr rep1 = vtkSplineRepresentationPtr::New();
//	//	node1->SetRepresentation(rep1);
//	//	node1->SetInteractor(view->getRenderWindow()->GetInteractor());
//
//	//	mBoxWidget = vtkBoxWidgetPtr::New();
//	//	mBoxWidget->RotationEnabledOff();
//	//
//	//	double bb_hard[6] =
//	//	{ -1, 1, -1, 1, -1, 1 };
//	//	mBoxWidget->PlaceWidget(bb_hard);
//
//}

}
