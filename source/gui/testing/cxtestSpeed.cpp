/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "cxLegacySingletons.h"
#include "cxMessageListener.h"
#include "cxViewService.h"

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
	RenderTesterPtr renderTester = cxtest::RenderTester::create(cx::viewService()->get3DView()->getRenderWindow());
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

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
}

TEST_CASE("Speed: vtkVolumeTextureMapper3D render, optimizedViews off", "[speed][gui][integration]")
{
	initTest();

	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkVolumeTextureMapper3D");
	cx::settings()->setValue("optimizedViews", false);

	int fps = calculateFPS(false);
	JenkinsMeasurement jenkins;
	jenkins.printMeasurementWithCxReporter("FPS_vtkVolumeTextureMapper3D", QString::number(fps));

	// TODO: enter this value into config file
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

	// TODO: enter this value into config file
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

	// TODO: enter this value into config file
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

	// TODO: enter this value into config file
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

	// TODO: enter this value into config file
//	double minimumFPS = 5;
//	REQUIRE(fps > minimumFPS);
	REQUIRE(true);
}
#endif

}//namespace cx
