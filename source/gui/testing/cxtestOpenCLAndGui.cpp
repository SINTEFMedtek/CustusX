/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifdef CX_USE_OPENCL_UTILITY

#include "catch.hpp"
#include "cxOpenCLPrinter.h"

#include <QApplication>
#include <QPushButton>
#include <QTimer>

#include <testing/cxTestRenderSpeed.h>

namespace cxtest
{

TEST_CASE("OpenCL and Gui: Init OpenCL and show a button", "[unit][OpenCL][gui][hide]")
{
	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

    QPushButton button ("Hello world !");
    button.show();

    QTimer::singleShot(1000, qApp, SLOT(quit()) );

    CHECK(true);
    qApp->exec();

}

TEST_CASE("OpenCL and Gui: Show a button and init OpenCL", "[unit][OpenCL][gui][hide]")
{
    QPushButton button ("Hello world !");
    button.show();

    QTimer::singleShot(1000, qApp, SLOT(quit()) );

	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

	CHECK(true);
    qApp->exec();
}

TEST_CASE("OpenCL and OpenGl: Show a cx::ViewWidget (OpenGL) and init OpenCL", "[unit][OpenCL][OpenGL][gui][hide][segmentation_fault]")
{
    TestRenderSpeed helper;
    helper.testSingleView();

	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

	CHECK(true);
}

TEST_CASE("OpenCL and OpenGl: Show a vtkRenderWindow (OpenGL) and init OpenCL", "[unit][OpenCL][OpenGL][gui][hide][segmentation_fault]")
{
		TestRenderWindowSpeed helper;
		helper.testVtkRenderWindow();

	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

	CHECK(true);
}

TEST_CASE("OpenCL and OpenGl: Init OpenCL and show a OpenGL view.", "[unit][OpenCL][OpenGL][gui][hide]")
{
	cx::OpenCLPrinter::printPlatformAndDeviceInfo();

    TestRenderSpeed helper;
    helper.testSingleView();

	CHECK(true);
}

}
#endif //CX_USE_OPENCL_UTILITY
