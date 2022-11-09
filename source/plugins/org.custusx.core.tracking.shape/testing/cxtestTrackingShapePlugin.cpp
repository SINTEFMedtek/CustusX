/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxReadFbgsMessage.h"
#include <vtkPolyData.h>

namespace cxtest {

class ReadFbgsMessageTest : public cx::ReadFbgsMessage
{
public:
	ReadFbgsMessageTest() :
		cx::ReadFbgsMessage()
	{}
	bool createPolyData()
	{
		return cx::ReadFbgsMessage::createPolyData();
	}
	std::vector<double> getAxisPosVector(AXIS axis)
	{
		return cx::ReadFbgsMessage::getAxisPosVector(axis);
	}
	void initAxisVectors()
	{
		mXaxis.push_back(0);
		mXaxis.push_back(1);
		mXaxis.push_back(1);
		mYaxis.push_back(0);
		mYaxis.push_back(1);
		mYaxis.push_back(1);
		mZaxis.push_back(0);
		mZaxis.push_back(1);
		mZaxis.push_back(1);
	}
};
TEST_CASE("TrackingShapePlugin: Check nothing", "[unit][plugins][org.custusx.tracking.shape][hide]")
{
	CHECK(true);
}
TEST_CASE("ReadFbgsMessage: createPolyData", "[unit][plugins][org.custusx.tracking.shape][hide]")
{
	ReadFbgsMessageTest readFbgsMessage;
	CHECK(readFbgsMessage.getAxisPosVector(cx::ReadFbgsMessage::axisX).size() == 0);
	readFbgsMessage.initAxisVectors();
	CHECK(readFbgsMessage.getAxisPosVector(cx::ReadFbgsMessage::axisX).size() == 3);

	CHECK(readFbgsMessage.createPolyData());
	CHECK(readFbgsMessage.getAxisPosVector(cx::ReadFbgsMessage::axisX).size() == 0);
	vtkPolyDataPtr polydata = readFbgsMessage.getPolyData();

	REQUIRE(polydata);
	CHECK(polydata->GetPoints()->GetNumberOfPoints() == 3);
	CHECK(polydata->GetNumberOfPoints() == 3);
}
}//cxtest
