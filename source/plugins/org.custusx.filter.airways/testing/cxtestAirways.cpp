/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxtestAirwaysFixture.h"
#ifdef CX_APPLE
#include <vtkCocoaRenderWindow.h>
#endif
#ifdef CX_WINDOWS
#include <vtkWin32OpenGLRenderWindow.h>
#endif


namespace cxtest
{

//Atm. this test doesn't have any test data. Therefore it is hidden.
TEST_CASE("Airway segmentation on test data pat011 works", "[integration][airways][hide]")
{
#ifdef CX_APPLE
	vtkCocoaRenderWindow* v = vtkCocoaRenderWindow::New();
	v->Initialize();
#endif
#ifdef CX_WINDOWS
    vtkWin32OpenGLRenderWindow* v = vtkWin32OpenGLRenderWindow::New();
    v->Initialize();
#endif

    AirwaysFixture helper;
	helper.testLungAirwaysCT();

#if defined (CX_APPLE) || defined (CX_WINDOWS)
	v->Delete();
#endif
}

} // namespace cxtest

