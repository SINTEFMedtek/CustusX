#include "catch.hpp"

#include "cxtestTestTransferFunctionColorWidget.h"

namespace cxtest
{

TEST_CASE("Correct ColorTFBoundaries calculated for default transfer function", "[unit][gui][widget]")
{
	TestTransferFunctionColorWidget*  widget = new TestTransferFunctionColorWidget();
	QRect plotArea = QRect(10, 20, 100, 200);
	widget->setPlotArea(plotArea);
	widget->initWithTestData();
	CHECK(plotArea.left() <= widget->getLeftAreaBoundary());
	CHECK(plotArea.right() >= widget->getRigthAreaBoundary());
	delete widget;
}

} //namespace cxtest
