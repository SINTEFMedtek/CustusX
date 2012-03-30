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

namespace cx
{

EraserWidget::EraserWidget(QWidget* parent) :
	BaseWidget(parent, "EraserWidget", "Eraser")
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	this->setToolTip(this->defaultWhatsThis());

	layout->addWidget(new QLabel("pang!"));
	layout->addStretch();

	this->createAction(this, QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"), "Test", "Test",
		SLOT(testSlot()), layout);
	this->createAction(this, QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"), "Remove", "Remove",
		SLOT(removeSlot()), layout);

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

void EraserWidget::removeSlot()
{
	vtkPolyDataPtr poly = vtkPolyDataPtr::New();
	mEraserSphere->GetPolyData(poly);

	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	vtkImageDataPtr img = image->getBaseVtkImageData();

	std::cout << "starting" << std::endl;

	Eigen::Array3i dim(img->GetDimensions());
	ssc::Vector3D targetSpacing(img->GetSpacing());
	unsigned char *outputPointer = static_cast<unsigned char*> (img->GetScalarPointer());

	//	ssc::Vector3D c(200,200,200);
	ssc::Vector3D c(mEraserSphere->GetCenter());
	double r = mEraserSphere->GetRadius();

	ssc::Transform3D dMr = image->get_rMd().inv();
	ssc::Transform3D rawMd = ssc::createTransformScale(targetSpacing).inv();
	ssc::Transform3D rawMr = rawMd * dMr;
	c = rawMr.coord(c);
	r = rawMr.vector(r * ssc::Vector3D::UnitX()).length();

	//	double r=50;
	//	ssc::Vector3D c(200,200,200);

	for (int x = 0; x < dim[0]; ++x)
		for (int y = 0; y < dim[1]; ++y)
			for (int z = 0; z < dim[2]; ++z)
			{
				int index = x + y * dim[0] + z * dim[0] * dim[1];

				if ((ssc::Vector3D(x, y, z) - c).length() < r)
					outputPointer[index] = 255;
			}
	image->setVtkImageData(img);
}

void EraserWidget::testSlot()
{
	typedef vtkSmartPointer<vtkSplineWidget> vtkSplineWidgetPtr;
	typedef vtkSmartPointer<vtkSplineWidget2> vtkSplineWidget2Ptr;
	typedef vtkSmartPointer<vtkSplineRepresentation> vtkSplineRepresentationPtr;

	ssc::View* view = viewManager()->get3DView();
	mEraserSphere = vtkSphereWidget::New();
	mEraserSphere->SetInteractor(view->getRenderWindow()->GetInteractor());
	mEraserSphere->SetEnabled(true);
	int a = 50;
	mEraserSphere->PlaceWidget(-a, a, -a, a, -a, a);
	mEraserSphere->ScaleOn();

	mEraserSphere->SetThetaResolution(12);
	mEraserSphere->SetPhiResolution(12);

	////	vtkSplineWidget2Ptr node1 = vtkSplineWidget2Ptr::New();
	//	vtkSplineWidget2* node1 = vtkSplineWidget2::New();
	//	vtkSplineRepresentationPtr rep1 = vtkSplineRepresentationPtr::New();
	//	node1->SetRepresentation(rep1);
	//	node1->SetInteractor(view->getRenderWindow()->GetInteractor());

	//	mBoxWidget = vtkBoxWidgetPtr::New();
	//	mBoxWidget->RotationEnabledOff();
	//
	//	double bb_hard[6] =
	//	{ -1, 1, -1, 1, -1, 1 };
	//	mBoxWidget->PlaceWidget(bb_hard);

}

}
