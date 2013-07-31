#include "catch.hpp"

#include "cxtestTestTransferFunctionColorWidget.h"
#include "cxtestTestTransferFunctions.h"

namespace cxtest
{

TEST_CASE("Correct ColorTFBoundaries calculated for default transfer function", "[unit][gui][widget]")
{
	TestTransferFunctionColorWidget*  widget = new TestTransferFunctionColorWidget();
	QRect plotArea = QRect(10, 20, 100, 200);
	widget->setPlotArea(plotArea);
	widget->initWithTestData();
	REQUIRE(plotArea.left() <= widget->getLeftAreaBoundary());
	REQUIRE(plotArea.right() >= widget->getRigthAreaBoundary());
	delete widget;
}


TEST_CASE("Valid default 3D transfer function", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	REQUIRE(testObject->hasValid3DTransferFunction());
	delete testObject;
}

TEST_CASE("Valid default 2D transfer function / lookup table", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	REQUIRE(testObject->hasValid2DTransferFunction());
	delete testObject;
}

TEST_CASE("Faulty window width in 3D transfer function is evaluated as not valid", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	testObject->Corrupt3DTransferFunctionWindowWidth();
	REQUIRE(!testObject->hasValid3DTransferFunction());
	delete testObject;
}

TEST_CASE("Faulty window width in 2D transfer function / lookup table is evaluated as not valid", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	testObject->Corrupt2DTransferFunctionWindowWidth();
	REQUIRE(!testObject->hasValid2DTransferFunction());
	delete testObject;
}

TEST_CASE("Faulty window level in 3D transfer function is evaluated as not valid", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	testObject->Corrupt3DTransferFunctionWindowLevel();
	REQUIRE(!testObject->hasValid3DTransferFunction());
	delete testObject;
}

TEST_CASE("Faulty window level in 2D transfer function / lookup table is evaluated as not valid", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	testObject->Corrupt2DTransferFunctionWindowLevel();
	REQUIRE(!testObject->hasValid2DTransferFunction());
	delete testObject;
}

TEST_CASE("sscImage don't accept faulty 3D transfer function", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	testObject->setNewCorrupt3DTranferFunction();
	REQUIRE(testObject->hasValid3DTransferFunction());
	delete testObject;
}

TEST_CASE("sscImage dont't accept faulty 2D transfer function / lookup table", "[unit]")
{
	TestTransferFunctions* testObject = new TestTransferFunctions();
	testObject->setNewCorrupt2DTranferFunction();
	REQUIRE(testObject->hasValid2DTransferFunction());
	delete testObject;
}


} //namespace cxtest
