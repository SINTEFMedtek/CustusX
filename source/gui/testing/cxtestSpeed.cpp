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
#include "cxLegacySingletons.h"
#include "cxMessageListener.h"

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
	RenderTesterPtr renderTester = cxtest::RenderTester::create(cx::viewManager()->get3DView()->getRenderWindow());
	vtkImageDataPtr output = renderTester->renderToImage();
	int numNonZeroPixels = Utilities::getNumberOfNonZeroVoxels(output);
//	std::cout << "numNonZeroPixels: " << numNonZeroPixels << std::endl;
	REQUIRE(numNonZeroPixels > 50000);//Expect more than the pointer in the 3D scene
	REQUIRE(numNonZeroPixels < output->GetDimensions()[0]*output->GetDimensions()[1]);
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

TEST_CASE("Speed: vtkVolumeTextureMapper3D render", "[speed][gui][integration][not_win32]")
{
	initTest();
	cx::MessageListenerPtr messageListener = cx::MessageListener::create();

	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkVolumeTextureMapper3D");

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	int fps = calculateFPS(false);
	jenkins.createOutput("FPS_vtkVolumeTextureMapper3D", QString::number(fps));

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
	CHECK(!messageListener->containsErrors());
}

TEST_CASE("Speed: vtkGPUVolumeRayCastMapper render", "[speed][gui][integration][not_win32]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkGPUVolumeRayCastMapper");

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	int fps = calculateFPS(false);
	jenkins.createOutput("FPS_vtkGPUVolumeRayCastMapper", QString::number(fps));

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
}

TEST_CASE("Speed: vtkGPUVolumeRayCastMapper with slicing", "[speed][gui][integration][not_win32]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkGPUVolumeRayCastMapper");

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	bool slicing = true;
	int fps = calculateFPS(slicing);
	jenkins.createOutput("FPS_vtkGPUVolumeRayCastMapper_Slicing", QString::number(fps));

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(fps > minimumFPS);
}


TEST_CASE("Speed: vtkOpenGLGPUMultiVolumeRayCastMapper renderer", "[speed][gui][integration][not_win32][not_win64][unstable]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkOpenGLGPUMultiVolumeRayCastMapper");

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	int fps = calculateFPS(false);
	jenkins.createOutput("FPS_vtkOpenGLGPUMultiVolumeRayCastMapper", QString::number(fps));

	// TODO: enter this value into config file
//	double minimumFPS = 5;
//	REQUIRE(fps > minimumFPS);
	REQUIRE(true);
}

TEST_CASE("Speed: vtkOpenGLGPUMultiVolumeRayCastMapper with slicing", "[speed][gui][integration][not_win32][not_win64][unstable]")
{
	initTest();
	cx::settings()->setValue("View3D/ImageRender3DVisualizer", "vtkOpenGLGPUMultiVolumeRayCastMapper");

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	bool slicing = true;
	int fps = calculateFPS(slicing);
	jenkins.createOutput("FPS_vtkOpenGLGPUMultiVolumeRayCastMapper_Slicing", QString::number(fps));

	// TODO: enter this value into config file
//	double minimumFPS = 5;
//	REQUIRE(fps > minimumFPS);
	REQUIRE(true);
}


}//namespace cx
