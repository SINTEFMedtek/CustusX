/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QTimer>
#include <vtkImageData.h>

#include "cxImage.h"
#include "cxView.h"
#include "cxtestSender.h"
#include "cxtestQueuedSignalListener.h"
#include "cxtestUtilities.h"
#include "cxTestCustusXController.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxtestJenkinsMeasurement.h"
#include "cxtestRenderTester.h"
#include "cxMessageListener.h"
#include "cxViewService.h"
#include "cxTestRenderSpeed.h"

namespace cxtest
{

namespace
{
void initTest()
{
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("renderingInterval", 4);
}

void requireVolumeIn3DScene()
{
	RenderTesterPtr renderTester = cxtest::RenderTester::create(cx::logicManager()->getViewService()->get3DView()->getRenderWindow());
	vtkImageDataPtr output = renderTester->renderToImage();
	double fractionNonZeroPixels = Utilities::getFractionOfVoxelsAboveThreshold(output, 0);

	REQUIRE(fractionNonZeroPixels > 0.19);//Expect more than the pointer in the 3D scene
}

double calculateFPS(bool slicing)
{
	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";
	custusX.mEnableSlicing = slicing;

	custusX.start();
	qApp->exec();
	requireVolumeIn3DScene();
	custusX.stop();

	return custusX.mMeasuredFPS;
}

}

TEST_CASE("CustusX full run emits no errors, correct service shutdown.", "[integration][not_win32][not_win64]")
{
	initTest();
	cx::Reporter::initialize();
	cx::MessageListenerPtr messageListener = cx::MessageListener::createWithQueue();

	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkVolumeTextureMapper3D");
	cx::settings()->setValue("Automation/autoStartStreaming", "false");

	CustusXController custusX(NULL);
	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";
	custusX.mBaseTime = 1;
	custusX.start();
	qApp->exec();
	custusX.stop();

	// the original argument for this test was to check if LogicManager succeeds in deleting
	// all services: Failure to do so sends an error to the Reporter.
	CHECK(!messageListener->containsErrors());
}

/**
 * This test has BIG problems when run on a test box, with screen saver on etc etc. Removed from Linux because of this
 */
//TEST_CASE("Speed: vtkVolumeTextureMapper3D render", "[speed][gui][integration][not_win32][not_win64]")
TEST_CASE("Speed: vtkVolumeTextureMapper3D render, optimizedViews on", "[speed][gui][integration]")
{
	initTest();

	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkVolumeTextureMapper3D");
	cx::settings()->setValue("optimizedViews", true);

	int fps = calculateFPS(false);
	JenkinsMeasurement jenkins;
	jenkins.printMeasurementWithCxReporter("FPS_vtkVolumeTextureMapper3D", QString::number(fps));

	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
}

TEST_CASE("Speed: vtkVolumeTextureMapper3D render, optimizedViews off", "[speed][gui][integration]")
{
	initTest();

	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkVolumeTextureMapper3D");
	cx::settings()->setValue("optimizedViews", false);
	cx::settings()->setValue("View3D/showManualTool", false); //We set this to false to make the test pass on Linux in Fraxinus. See JIRA FX-8 for more details.

	int fps = calculateFPS(false);
	JenkinsMeasurement jenkins;
	jenkins.printMeasurementWithCxReporter("FPS_vtkVolumeTextureMapper3D", QString::number(fps));

	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
}

/**
 * This test has BIG problems when run on a test box, with screen saver on etc etc. Removed from Linux because of this
 */
TEST_CASE("Speed: vtkGPUVolumeRayCastMapper render", "[speed][gui][integration]")
//TEST_CASE("Speed: vtkGPUVolumeRayCastMapper render", "[speed][gui][integration][not_win32][not_win64][not_mavericks]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkGPUVolumeRayCastMapper");

	int fps = calculateFPS(false);

	JenkinsMeasurement jenkins;
	jenkins.printMeasurementWithCxReporter("FPS_vtkGPUVolumeRayCastMapper", QString::number(fps));

	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
}

/**
 * This test has BIG problems when run on a test box, with screen saver on etc etc. Removed from Linux because of this
 */
TEST_CASE("Speed: vtkGPUVolumeRayCastMapper with slicing", "[speed][gui][integration]")
//TEST_CASE("Speed: vtkGPUVolumeRayCastMapper with slicing", "[speed][gui][integration][not_win32][not_win64][not_mavericks][not_linux]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkGPUVolumeRayCastMapper");

	bool slicing = true;
	int fps = calculateFPS(slicing);
	JenkinsMeasurement jenkins;
	jenkins.printMeasurementWithCxReporter("FPS_vtkGPUVolumeRayCastMapper_Slicing", QString::number(fps));

	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
}

#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
TEST_CASE("Speed: vtkOpenGLGPUMultiVolumeRayCastMapper renderer", "[speed][gui][integration][not_win32][not_win64][unstable]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkOpenGLGPUMultiVolumeRayCastMapper");

	int fps = calculateFPS(false);
	JenkinsMeasurement jenkins;
	jenkins.printMeasurementWithCxReporter("FPS_vtkOpenGLGPUMultiVolumeRayCastMapper", QString::number(fps));

//	double minimumFPS = 5;
//	REQUIRE(fps > minimumFPS);
	REQUIRE(true);
}
#endif

#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
TEST_CASE("Speed: vtkOpenGLGPUMultiVolumeRayCastMapper with slicing", "[speed][gui][integration][not_win32][not_win64][unstable]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkOpenGLGPUMultiVolumeRayCastMapper");

	bool slicing = true;
	int fps = calculateFPS(slicing);
	JenkinsMeasurement jenkins;
	jenkins.printMeasurementWithCxReporter("FPS_vtkOpenGLGPUMultiVolumeRayCastMapper_Slicing", QString::number(fps));

//	double minimumFPS = 5;
//	REQUIRE(fps > minimumFPS);
	REQUIRE(true);
}
#endif


TEST_CASE("Speed: Render time of vtkRenderWindow", "[speed]")
{
	initTest();
	cx::settings()->setValue("optimizedViews", false);
	TestRenderSpeed helper;

	QTime clock;

	int numRenders = 10;
	int numViews = 1;

//	helper.testSingleView();
	helper.createViews(numViews);
	helper.showViews();
	clock.start();
	helper.renderNumTimes(numRenders);
	int timeMs = clock.elapsed();
	std::cout << "render "<< numViews << " views " << numRenders << " times: " << timeMs << " ms" << std::endl;
	std::cout << timeMs/double(numRenders*numViews) << " ms per render" << std::endl;


	TestRenderSpeed helper2;
	numViews = 100;

//	helper.testSeveralViews();
	helper2.createViews(numViews);
	helper2.showViews();
	clock.start();
	helper2.renderNumTimes(numRenders);
	timeMs = clock.elapsed();
	std::cout << "render "<< numViews << " views " << numRenders << " times: " << timeMs << " ms" << std::endl;
	std::cout << timeMs/double(numRenders*numViews) << " ms per render" << std::endl;

	CHECK(true);
}

}//namespace cx
