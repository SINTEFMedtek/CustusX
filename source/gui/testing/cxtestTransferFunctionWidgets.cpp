/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
