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

#include "cxtestTestTransferFunctionColorWidget.h"
#include "cxtestTestTransferFunctions.h"
#include "cxPatientModelService.h"
#include "cxSessionStorageService.h"
#include "cxActiveData.h"

namespace cxtest
{

TEST_CASE("Correct ColorTFBoundaries calculated for default transfer function", "[unit][gui][not_win32][widget]")
{
	cx::ActiveDataPtr mockActiveData = cx::ActiveDataPtr(new cx::ActiveData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject()));
	TestTransferFunctionColorWidget*  widget = new TestTransferFunctionColorWidget(mockActiveData);
	QRect plotArea = QRect(10, 20, 100, 200);
	widget->setPlotArea(plotArea);
	widget->initWithTestData();
	REQUIRE(plotArea.left() <= widget->getLeftAreaBoundary());
	REQUIRE(plotArea.right() >= widget->getRigthAreaBoundary());
	delete widget;
}


//The following tests only test the transfer functions of a sscImage, and should probably be moved

//TEST_CASE("Valid default 3D transfer function", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	REQUIRE(testObject->hasValid3DTransferFunction());
//	delete testObject;
//}

//TEST_CASE("Valid default 2D transfer function / lookup table", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	REQUIRE(testObject->hasValid2DTransferFunction());
//	delete testObject;
//}

//TEST_CASE("Faulty window width in 3D transfer function is evaluated as not valid", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	testObject->Corrupt3DTransferFunctionWindowWidth();
//	REQUIRE(!testObject->hasValid3DTransferFunction());
//	delete testObject;
//}

//TEST_CASE("Faulty window width in 2D transfer function / lookup table is evaluated as not valid", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	testObject->Corrupt2DTransferFunctionWindowWidth();
//	REQUIRE(!testObject->hasValid2DTransferFunction());
//	delete testObject;
//}

//TEST_CASE("Faulty window level in 3D transfer function is evaluated as not valid", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	testObject->Corrupt3DTransferFunctionWindowLevel();
//	REQUIRE(!testObject->hasValid3DTransferFunction());
//	delete testObject;
//}

//TEST_CASE("Faulty window level in 2D transfer function / lookup table is evaluated as not valid", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	testObject->Corrupt2DTransferFunctionWindowLevel();
//	REQUIRE(!testObject->hasValid2DTransferFunction());
//	delete testObject;
//}

//TEST_CASE("sscImage don't accept faulty 3D transfer function", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	testObject->setNewCorrupt3DTranferFunction();
//	REQUIRE(testObject->hasValid3DTransferFunction());
//	delete testObject;
//}

//TEST_CASE("sscImage dont't accept faulty 2D transfer function / lookup table", "[unit]")
//{
//	TestTransferFunctions* testObject = new TestTransferFunctions();
//	testObject->setNewCorrupt2DTranferFunction();
//	REQUIRE(testObject->hasValid2DTransferFunction());
//	delete testObject;
//}


} //namespace cxtest
