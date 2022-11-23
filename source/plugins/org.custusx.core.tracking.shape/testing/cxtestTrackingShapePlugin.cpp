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
#include "cxVisServices.h"
#include "cxVector3D.h"
#include "cxMesh.h"

namespace cxtest {

class ReadFbgsMessageTest : public cx::ReadFbgsMessage
{
public:
	cx::Transform3D mShapePointLockTransform = cx::Transform3D::Identity();

	ReadFbgsMessageTest() :
		cx::ReadFbgsMessage(cx::VisServices::getNullObjects())
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

	cx::Transform3D lockShape(int pos)
	{
		mShapePointLockTransform = cx::ReadFbgsMessage::lockShape(pos);
		return mShapePointLockTransform;
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

	cx::Vector3D getDeltaPosition(int pos)
	{
		return cx::ReadFbgsMessage::getDeltaPosition((pos));
	}

	void setRangeMax(int range)
	{
		mRangeMax = range;
	}
	bool getMeshAdded()
	{
		return mMeshAdded;
	}
	cx::Transform3D getPrMt()
	{
		return m_prMt;
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

	CHECK_FALSE(readFbgsMessage.createPolyData());
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
	cx::Transform3D identity = cx::Transform3D::Identity();
	CHECK(cx::similar(readFbgsMessage.mShapePointLockTransform, identity));
	QString buffer = readFbgsMessage.bufferWithThreeValues();
	readFbgsMessage.setShapePointLock(1);
	readFbgsMessage.readBuffer(buffer);

	vtkPolyDataPtr polydata = readFbgsMessage.getPolyData();
	REQUIRE(polydata);
	CHECK(polydata->GetNumberOfPoints() == 3);

	CHECK_FALSE(cx::similar(readFbgsMessage.mShapePointLockTransform, identity));

	readFbgsMessage.setShapePointLock(2);
	readFbgsMessage.readBuffer(buffer);
	CHECK_FALSE(cx::similar(readFbgsMessage.mShapePointLockTransform, identity));
}

TEST_CASE("ReadFbgsMessage: getDeltaPosition", "[unit][plugins][org.custusx.tracking.shape]")
{
	ReadFbgsMessageTest readFbgsMessage;
	readFbgsMessage.setRangeMax(1);
	cx::Vector3D delta_p = readFbgsMessage.getDeltaPosition(1);
	CHECK(delta_p == cx::Vector3D(0,0,1));


	QString buffer = readFbgsMessage.bufferWithThreeValues();
	readFbgsMessage.readBuffer(buffer);
	CHECK(readFbgsMessage.getRangeMax() == 3-1);//mRangeMax == 3, but getRangeMax() subtrackts 1

	cx::Vector3D delta_p_max_3 = readFbgsMessage.getDeltaPosition(1);
	CHECK(delta_p_max_3 != cx::Vector3D(0,0,1));

	readFbgsMessage.setRangeMax(2);
	cx::Vector3D delta_p_max_2 = readFbgsMessage.getDeltaPosition(1);
	CHECK(delta_p_max_2 != cx::Vector3D(0,0,1));
	CHECK(delta_p_max_2 != delta_p_max_3);
}

TEST_CASE("ReadFbgsMessage: getMesh", "[unit][plugins][org.custusx.tracking.shape]")
{
	ReadFbgsMessageTest readFbgsMessage;
	CHECK_FALSE(readFbgsMessage.getMeshAdded());
	cx::MeshPtr mesh = readFbgsMessage.getMesh();
	CHECK(readFbgsMessage.getMeshAdded());
	REQUIRE(mesh);
	CHECK(mesh->getUid() == readFbgsMessage.getMeshUid());

	CHECK(mesh->getVtkPolyData() == readFbgsMessage.getPolyData());
}

TEST_CASE("ReadFbgsMessage: saveMeshSnapshot", "[unit][plugins][org.custusx.tracking.shape]")
{
	ReadFbgsMessageTest readFbgsMessage;
	CHECK_FALSE(readFbgsMessage.saveMeshSnapshot());

	cx::MeshPtr mesh = readFbgsMessage.getMesh();
	CHECK(readFbgsMessage.saveMeshSnapshot());
}

TEST_CASE("ReadFbgsMessage: set_prMt", "[unit][plugins][org.custusx.tracking.shape]")
{
	ReadFbgsMessageTest readFbgsMessage;
	CHECK(cx::similar(readFbgsMessage.getPrMt(), cx::Transform3D::Identity()));
	cx::Transform3D prMt = cx::Transform3D(cx::createTransformTranslate(cx::Vector3D(2,2,2)));
	readFbgsMessage.set_prMt(prMt);
	CHECK(cx::similar(readFbgsMessage.getPrMt(), prMt));
}
}//cxtest
