/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "catch.hpp"
#include "cxRenderWindowFactory.h"
#include "cxSharedOpenGLContext.h"
#include <vtkRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>
#include <QString>

namespace cxtest {

TEST_CASE("RenderWindowFactory init", "[opengl][resource][visualization][integration]")
{
    cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
    REQUIRE(renderWindowFactory);
}

TEST_CASE("RenderWindowFactory init shared render window", "[opengl][resource][visualization][integration]")
{
    cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	REQUIRE(renderWindowFactory->getSharedRenderWindow());
	REQUIRE(renderWindowFactory->getSharedOpenGLContext());
}

TEST_CASE("RenderWindowFactory get render window", "[opengl][resource][visualization][integration]")
{
    cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
    QString uid = "TestWindowUid1";
	vtkRenderWindowPtr renderWindow = renderWindowFactory->getRenderWindow(uid);
	REQUIRE(renderWindow);

	vtkRenderWindowPtr renderWindowCopy = renderWindowFactory->getRenderWindow(uid);

	REQUIRE (renderWindow == renderWindowCopy);
}

TEST_CASE("RenderWindowFactory render", "[opengl][resource][visualization][integration]")
{
	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	REQUIRE(renderWindow);
	renderWindow->Render();
}

TEST_CASE("RenderWindowFactory render in render window", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	QString uid = "TestWindowUid1";
	vtkRenderWindowPtr renderWindow = renderWindowFactory->getRenderWindow(uid, false);
	REQUIRE(renderWindow);
	renderWindow->Render();
}

//This test requires only one OpenGL context, so it have to run by itself (run as integration)
TEST_CASE("RenderWindowFactory render a single renderWindow", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	QString uid("cx_shared_context");
	REQUIRE(renderWindowFactory->renderWindowExists(uid));
	vtkRenderWindowPtr sharedRenderWindow1 = renderWindowFactory->getRenderWindow(uid);
	REQUIRE(sharedRenderWindow1);

	vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(sharedRenderWindow1.Get());
	REQUIRE(opengl_renderwindow);
	REQUIRE(cx::SharedOpenGLContext::isValid(opengl_renderwindow));

	vtkRenderWindowPtr sharedRenderWindow2 = renderWindowFactory->getSharedRenderWindow();
	CHECK(sharedRenderWindow2);
	REQUIRE(sharedRenderWindow2 == sharedRenderWindow1);

	QString testUid("testUid");
	REQUIRE_FALSE(renderWindowFactory->renderWindowExists(testUid));
	vtkRenderWindowPtr renderWindow2 = renderWindowFactory->getRenderWindow(testUid);
	REQUIRE(renderWindow2);

	vtkOpenGLRenderWindowPtr opengl_renderwindow2 = vtkOpenGLRenderWindow::SafeDownCast(renderWindow2.Get());
	REQUIRE(opengl_renderwindow2);
	REQUIRE(cx::SharedOpenGLContext::isValid(opengl_renderwindow2));

	REQUIRE(renderWindowFactory->renderWindowExists(testUid));
	for(int i = 0; i < 10; ++i)
	{
		renderWindow2->Render();
		sharedRenderWindow2->Render();
	}
}

//Some code copied from RenderWindowFactory::createRenderWindow
TEST_CASE("Speed: Render time of vtkOpenGLRenderWindow", "[speed]")
{
	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow);
	REQUIRE(opengl_renderwindow);

	QTime clock;
	clock.start();
//	opengl_renderwindow->OpenGLInit();
	opengl_renderwindow->Render();
	int timeMs = clock.elapsed();
	std::cout << "Time for calling first vtkOpenGLRenderWindow::Render(): " << timeMs << " ms." << std::endl;
//	std::cout << "Time for calling vtkOpenGLRenderWindow::OpenGLInit(): " << timeMs << " ms." << std::endl;

	std::vector<vtkOpenGLRenderWindowPtr> openGLRenderWindows;

	int numRenderWindows = 50;
	for (int i = 0; i < numRenderWindows; ++i)
	{
		vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
		vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow);
		REQUIRE(opengl_renderwindow);
		openGLRenderWindows.push_back(opengl_renderwindow);
	}

	clock.start();
	for (int i = 0; i < openGLRenderWindows.size(); ++i)
	{
		openGLRenderWindows[i]->Render();
	}
	timeMs = clock.elapsed();
	std::cout << "Time for calling first vtkOpenGLRenderWindow::Render() on " << numRenderWindows << " render windows: " << timeMs << " ms." << std::endl;
	std::cout << "Time per init+render: " << timeMs / double(numRenderWindows) << " ms." << std::endl;

	clock.start();
	for (int i = 0; i < openGLRenderWindows.size(); ++i)
	{
		openGLRenderWindows[i]->Render();
	}
	timeMs = clock.elapsed();
	std::cout << "Time for calling second vtkOpenGLRenderWindow::Render() on " << numRenderWindows << " render windows: " << timeMs << " ms." << std::endl;
	std::cout << "Time per render: " << timeMs / double(numRenderWindows) << " ms." << std::endl;

}


}//cxtest
