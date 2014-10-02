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
