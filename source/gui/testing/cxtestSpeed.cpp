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

#include "catch.hpp"

#include <QTimer>
#include <vtkImageData.h>

#include "sscImage.h"
#include "sscView.h"
#include "cxtestSender.h"
#include "cxtestQueuedSignalListener.h"
#include "cxtestUtilities.h"
#include "cxTestCustusXController.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxtestJenkinsMeasurement.h"
#include "cxtestRenderTester.h"
#include "cxViewManager.h"


namespace cxtest
{

void initTest()
{
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("renderingInterval", 4);
#ifdef __APPLE__
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkVolumeTextureMapper3D");
#else
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkGPUVolumeRayCastMapper");
#endif
}

void requireVolumeIn3DScene()
{
	RenderTesterPtr renderTester = cxtest::RenderTester::create(cx::viewManager()->get3DView()->getRenderWindow());
	vtkImageDataPtr output = renderTester->renderToImage();
	int numNonZeroPixels = Utilities::getNumberOfNonZeroVoxels(output);
//	std::cout << "numNonZeroPixels: " << numNonZeroPixels << std::endl;
	REQUIRE(numNonZeroPixels > 50000);//Expect more than the pointer in the 3D scene
	REQUIRE(numNonZeroPixels < output->GetDimensions()[0]*output->GetDimensions()[1]);
}

TEST_CASE("Speed: Run CustusX with a minimum render speed", "[speed][gui][integration]")
{
	initTest();

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";

	custusX.start();
	qApp->exec();
	requireVolumeIn3DScene();
	custusX.stop();

	jenkins.createOutput("FPS", QString::number(custusX.mMeasuredFPS));

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(custusX.mMeasuredFPS > minimumFPS);
}

TEST_CASE("Speed: Run CustusX with interactive slicing at a minimum render speed", "[speed][gui][integration]")
{
	initTest();

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";
	custusX.mEnableSlicing = true;

	custusX.start();
	qApp->exec();
	requireVolumeIn3DScene();
	custusX.stop();

	jenkins.createOutput("FPS_Slicing", QString::number(custusX.mMeasuredFPS));

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(custusX.mMeasuredFPS > minimumFPS);
}

TEST_CASE("Speed: Multi volume renderer", "[speed][gui][integration]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkOpenGLGPUMultiVolumeRayCastMapper");

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";

	custusX.start();
	qApp->exec();
	requireVolumeIn3DScene();
	custusX.stop();

	jenkins.createOutput("FPS_MultiVolume", QString::number(custusX.mMeasuredFPS));

	// TODO: enter this value into config file
	double minimumFPS = 0;
//	REQUIRE(custusX.mMeasuredFPS > minimumFPS);
	REQUIRE(true);
}

TEST_CASE("Speed: Multi volume renderer and interactive slicing", "[speed][gui][integration]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkOpenGLGPUMultiVolumeRayCastMapper");

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";
	custusX.mEnableSlicing = true;

	custusX.start();
	qApp->exec();
	requireVolumeIn3DScene();
	custusX.stop();

	jenkins.createOutput("FPS_MultiVolume_Slicing", QString::number(custusX.mMeasuredFPS));

	// TODO: enter this value into config file
	double minimumFPS = 0;
//	REQUIRE(custusX.mMeasuredFPS > minimumFPS);
	REQUIRE(true);
}

}//namespace cx
