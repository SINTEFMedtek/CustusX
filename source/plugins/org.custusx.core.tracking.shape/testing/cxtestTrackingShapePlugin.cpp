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
#include "cxLogger.h"

namespace cxtest {

class ReadFbgsMessageTest : public cx::ReadFbgsMessage
{
public:
	cx::Vector3D mShapePointLockVector = cx::Vector3D(0, 0, 0);

	ReadFbgsMessageTest() :
		cx::ReadFbgsMessage()
	{}
	bool createPolyData()
	{
		return cx::ReadFbgsMessage::createPolyData();
	}
	std::vector<double> getAxisPosVector(AXIS axis)
	{
		return *cx::ReadFbgsMessage::getAxisPosVector(axis);
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

	QString getAxisString(AXIS axis)
	{
		return cx::ReadFbgsMessage::getAxisString(axis);
	}

	cx::Vector3D lockShape(int pos)
	{
		mShapePointLockVector = cx::ReadFbgsMessage::lockShape(pos);
		return mShapePointLockVector;
	}

	QString bufferWithOneValue()
	{
		//Test both float and int values for positions
		QString buffer;
		buffer += getAxisString(cx::ReadFbgsMessage::axisX);
		buffer += "	1	0	";
		buffer += getAxisString(cx::ReadFbgsMessage::axisY);
		buffer += "	1	0	";
		buffer += getAxisString(cx::ReadFbgsMessage::axisZ);
		buffer += "	1	0.0	";
		return buffer;
	}
	QString bufferWithThreeValues()
	{
		//Test both float and int values for positions
		QString buffer;
		buffer += getAxisString(cx::ReadFbgsMessage::axisX) + "	3	";
		buffer += "0	1	3	";
		buffer += getAxisString(cx::ReadFbgsMessage::axisY) + "	3	";
		buffer += "0	1	3	";
		buffer += getAxisString(cx::ReadFbgsMessage::axisZ) + "	3	";
		buffer += "0	1	3	";
		return buffer;
	}
};

TEST_CASE("ReadFbgsMessage: createPolyData", "[unit][plugins][org.custusx.tracking.shape]")
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

TEST_CASE("ReadFbgsMessage: readBuffer with incomplete data", "[unit][plugins][org.custusx.tracking.shape]")
{
	ReadFbgsMessageTest readFbgsMessage;
	QString buffer;
	//Empty buffer
	readFbgsMessage.readBuffer(buffer);

	//Unfinised buffer
	buffer += readFbgsMessage.getAxisString(cx::ReadFbgsMessage::axisX);
	readFbgsMessage.readBuffer(buffer);
	buffer += '	';
	readFbgsMessage.readBuffer(buffer);
	buffer += "1";
	readFbgsMessage.readBuffer(buffer);
	buffer += "	";
	readFbgsMessage.readBuffer(buffer);
	buffer += "0	";
	readFbgsMessage.readBuffer(buffer);
	CHECK(true);

}

TEST_CASE("ReadFbgsMessage: readBuffer with simple data", "[unit][plugins][org.custusx.tracking.shape]")
{
	ReadFbgsMessageTest readFbgsMessage;
	vtkPolyDataPtr polydata = readFbgsMessage.getPolyData();
	CHECK(polydata);
	QString buffer = readFbgsMessage.bufferWithOneValue();
	readFbgsMessage.readBuffer(buffer);
	CHECK(true);

	polydata = readFbgsMessage.getPolyData();
	REQUIRE(polydata);
	CHECK(polydata->GetNumberOfPoints() == 1);

	CHECK(readFbgsMessage.getRangeMax() == 0);
}

TEST_CASE("ReadFbgsMessage: Lock specific point to tool", "[unit][plugins][org.custusx.tracking.shape]")
{
	ReadFbgsMessageTest readFbgsMessage;
	CHECK(readFbgsMessage.mShapePointLockVector == cx::Vector3D(0, 0, 0));
	QString buffer = readFbgsMessage.bufferWithThreeValues();
	readFbgsMessage.setShapePointLock(1);
	readFbgsMessage.readBuffer(buffer);

	vtkPolyDataPtr polydata = readFbgsMessage.getPolyData();
	REQUIRE(polydata);
	CHECK(polydata->GetNumberOfPoints() == 3);

	CHECK(readFbgsMessage.mShapePointLockVector == cx::Vector3D(10, 10, 10));//In data is in cm, while out data for CX in is mm

	readFbgsMessage.setShapePointLock(2);
	readFbgsMessage.readBuffer(buffer);
	CHECK(readFbgsMessage.mShapePointLockVector == cx::Vector3D(30, 30, 30));
}

}//cxtest
