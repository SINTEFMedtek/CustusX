/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <QtWidgets>
#include <QVTKOpenGLNativeWidget.h>

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"

#include "cxImage.h"
#include "cxAxesRep.h"
#include "cxVolumetricRep.h"
#include "cxSliceComputer.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include "cxToolRep3D.h"
#include "cxDummyToolManager.h"
#include "cxDummyTool.h"
#include "cxSliceProxy.h"
#include "cxSlicerRepSW.h"
#include "cxViewsFixture.h"
#include "catch.hpp"
#include "cxtestRenderTester.h"
#include "cxImageLUT2D.h"
#include "cxtestUtilities.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxMetaImageReader.h"


using cx::Vector3D;
using cx::Transform3D;

class ImageTestList
{
public:
	ImageTestList()
	{
		image.push_back("Person5/person5_t1_unsigned.mhd");
		image.push_back("Person5/person5_t2_unsigned.mhd");
		image.push_back("Person5/person5_flair_unsigned.mhd");
	}
	QStringList image;
};

TEST_CASE("Visual rendering: Init view",
			"[integration][resource][visualization]")
{
	cxtest::ViewsFixture fixture;
	REQUIRE(true);
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("Visual rendering: Overlapping gridlayout",
		  "[unit][resource][visualization][hide]")
{
	QWidget widget;
	QGridLayout* layout = new QGridLayout(&widget);
	widget.setLayout(layout);
	layout->setSpacing(0);
	layout->setMargin(0);

	QTextEdit* edit = new QTextEdit;
	edit->setText("Visual rendering: Overlapping gridlayout");
	layout->addWidget(edit, 0,0, 2,2);

	QLabel* label = new QLabel("Hello Mixed!");
	layout->addWidget(label, 2,0, 1,1);

	QLabel* label2 = new QLabel("Hello Quixed!");
	layout->addWidget(label2, 1,1, 1,1);

	for (unsigned i=0; i<3; ++i)
	{
		layout->setColumnStretch(i, 1);
		layout->setRowStretch(i, 1);
	}

	widget.show();
//	qApp->exec();
}

TEST_CASE("Visual rendering: Empty QVTKWidget",
		  "[unit][resource][visualization][hide]")
{
	QWidget mainWidget;
	QGridLayout* layout = new QGridLayout;
	layout->setSpacing(2);
	layout->setMargin(10);
	mainWidget.setLayout(layout);
	mainWidget.setGeometry(150,150,400,400);

	QVTKOpenGLNativeWidget* vtkWidget = new QVTKOpenGLNativeWidget();
	//	viewWidget clear
//  set custom renderwindow
//	viewWidget clear

	vtkWidget->renderWindow()->GetInteractor()->EnableRenderOff();
	vtkWidget->renderWindow()->GetInteractor()->Disable();
//	std::cout << "renderer count = " << vtkWidget->GetRenderWindow()->GetRenderers()->GetNumberOfItems() << std::endl;
//	vtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->Render();
	vtkRendererPtr renderer = vtkRendererPtr::New();
	renderer->SetBackground(0,0,0);
	vtkWidget->renderWindow()->AddRenderer(renderer);
	// dette bomber visualizeringa
//	vtkWidget->GetRenderWindow()->Render();

//	setstretchfactors
	layout->addWidget(vtkWidget, 0,0, 1,1);
//	main show
//	prettyzoom
//	render

	mainWidget.show();
	// did not work
//	QSize size = vtkWidget->size();
//	double factor = 2;
//	vtkWidget->GetRenderWindow()->SetSize(size.width()*factor, size.height()*factor);
	vtkWidget->renderWindow()->Render();

//	sleep(3);

	CHECK(true);
	//sleep(3);
	cxtest::Utilities::sleep_sec(3);
}

//Moved to unstable, as test sometimes fail
TEST_CASE("Visual rendering: Empty view",
		  "[unstable][resource][visualization]")
{
	cxtest::ViewsFixture fixture;

	cx::ViewPtr view = fixture.addView(0, 0);
//fixture.runWidget();
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("emptyview", 0);
	REQUIRE(fixture.getFractionOfBrightPixelsInRenderWindowForView(0,0) == Approx(0));
	
	CHECK_FALSE(fixture.messageListenerContainErrors());
//	sleep(3);

}

//Moved to unstable, as test sometimes fail
TEST_CASE("Visual rendering: Several empty views in a sequence.",
		  "[unstable][resource][visualization]")
{
	int numberOfIterations = 2;
	for (unsigned i=0; i<numberOfIterations; ++i)
	{
		cxtest::ViewsFixture fixture;

		cx::ViewPtr view = fixture.addView(0, 0);
		REQUIRE(fixture.quickRunWidget());

		fixture.dumpDebugViewToDisk("emptyview", 0);
		REQUIRE(fixture.getFractionOfBrightPixelsInRenderWindowForView(0,0) == Approx(0));
		CHECK_FALSE(fixture.messageListenerContainErrors());
	}
}

TEST_CASE("Visual rendering: Show 3D volume - vtkGPU render",
			"[integration][resource][visualization][not_win32][not_win64][unstable]")
{
	cxtest::ViewsFixture fixture;
	cx::FileReaderWriterServicePtr metaImageReader = cx::FileReaderWriterServicePtr(new cx::MetaImageReader(fixture.getPatientModelService()));
	fixture.addFileReaderWriter(metaImageReader);
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 0);

	REQUIRE(fixture.quickRunWidget());
	fixture.dumpDebugViewToDisk("3DvtkGPU", 0);
	REQUIRE(fixture.getFractionOfBrightPixelsInRenderWindowForView(0,0) > 0.01);
	
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("Visual rendering: Show ACS+3D, centered hidden tool",
			"[integration][resource][visualization][not_win32][not_win64][not_mavericks]")
{
	cxtest::ViewsFixture fixture;
	cx::FileReaderWriterServicePtr metaImageReader = cx::FileReaderWriterServicePtr(new cx::MetaImageReader(fixture.getPatientModelService()));
	fixture.addFileReaderWriter(metaImageReader);
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 1, 1);
	fixture.defineSlice("A", imagenames.image[0], cx::ptAXIAL, 0, 0);
	fixture.defineSlice("C", imagenames.image[0], cx::ptCORONAL, 1, 0);
	fixture.defineSlice("S", imagenames.image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(fixture.quickRunWidget());

//	REQUIRE(fixture.runWidget());

	fixture.dumpDebugViewToDisk("acs3d0", 0);
	fixture.dumpDebugViewToDisk("acs3d1", 1);
	fixture.dumpDebugViewToDisk("acs3d2", 2);
	fixture.dumpDebugViewToDisk("acs3d3", 3);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(2,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(3,20) > 0.02);
	
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("Visual rendering: Show layout, clear, show new layout",
			"[integration][resource][visualization][not_win32][not_win64][hide]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 1, 1);
	fixture.defineSlice("A", imagenames.image[0], cx::ptAXIAL, 0, 0);
	fixture.defineSlice("C", imagenames.image[0], cx::ptCORONAL, 1, 0);
	fixture.defineSlice("S", imagenames.image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(fixture.quickRunWidget());
	cxtest::Utilities::sleep_sec(1);


	std::cout << "===== CLEAR START" << std::endl;

	fixture.clear();
	std::cout << "===== CLEAR STOP" << std::endl;
	REQUIRE(fixture.quickRunWidget());
	cxtest::Utilities::sleep_sec(1);
	//    return;

	std::cout << "===== DEFINE 2 START" << std::endl;
	fixture.define3D(imagenames.image[0], NULL, 0, 2);
	fixture.defineSlice("Any", imagenames.image[0], cx::ptANYPLANE, 0, 0);
	fixture.defineSlice("Dua", imagenames.image[0], cx::ptSIDEPLANE, 0, 1);

	std::cout << "===== DEFINE 2 STOP" << std::endl;
	REQUIRE(fixture.quickRunWidget());
	//    REQUIRE(fixture.quickRunWidget());
	REQUIRE(fixture.runWidget());
	cxtest::Utilities::sleep_sec(1);

	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(2,20) > 0.02);
	
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("Visual rendering: Show AnyDual+3D, centered hidden tool",
			"[integration][resource][visualization][not_win32][not_win64][not_mavericks]")
{
	cxtest::ViewsFixture fixture;
	cx::FileReaderWriterServicePtr metaImageReader = cx::FileReaderWriterServicePtr(new cx::MetaImageReader(fixture.getPatientModelService()));
	fixture.addFileReaderWriter(metaImageReader);

	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 2);
	fixture.defineSlice("Any", imagenames.image[0], cx::ptANYPLANE, 0, 0);
	fixture.defineSlice("Dua", imagenames.image[0], cx::ptSIDEPLANE, 0, 1);
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("anydual3d0", 0);
	fixture.dumpDebugViewToDisk("anydual3d1", 1);
	fixture.dumpDebugViewToDisk("anydual3d2", 2);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(2,20) > 0.02);
	
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("Visual rendering: Show 3D+AnyDual, centered hidden tool",
			"[integration][resource][visualization][not_win32][not_win64][not_mavericks]")
{
	cxtest::ViewsFixture fixture;
	cx::FileReaderWriterServicePtr metaImageReader = cx::FileReaderWriterServicePtr(new cx::MetaImageReader(fixture.getPatientModelService()));
	fixture.addFileReaderWriter(metaImageReader);

	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 0);
	fixture.defineSlice("Any", imagenames.image[0], cx::ptANYPLANE, 0, 1);
	fixture.defineSlice("Dua", imagenames.image[0], cx::ptSIDEPLANE, 0, 2);
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("anydual3d0", 0);
	fixture.dumpDebugViewToDisk("anydual3d1", 1);
	fixture.dumpDebugViewToDisk("anydual3d2", 2);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(2,20) > 0.02);
	
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("Visual rendering: Show ACS, 3 volumes",
			"[integration][resource][visualization]")
{
	cxtest::ViewsFixture fixture;
	cx::FileReaderWriterServicePtr metaImageReader = cx::FileReaderWriterServicePtr(new cx::MetaImageReader(fixture.getPatientModelService()));
	fixture.addFileReaderWriter(metaImageReader);

	ImageTestList imagenames;

	for (unsigned i = 0; i < 3; ++i)
	{
		fixture.defineSlice("A", imagenames.image[i], cx::ptAXIAL, 0, i);
		fixture.defineSlice("C", imagenames.image[i], cx::ptCORONAL, 1, i);
		fixture.defineSlice("S", imagenames.image[i], cx::ptSAGITTAL, 2, i);
	}
	REQUIRE(fixture.quickRunWidget());

	for (unsigned i = 0; i < 3*3; ++i)
	{
		CHECK(fixture.getFractionOfBrightPixelsInRenderWindowForView(i,20) > 0.88);
	}
	
	CHECK_FALSE(fixture.messageListenerContainErrors());
}
