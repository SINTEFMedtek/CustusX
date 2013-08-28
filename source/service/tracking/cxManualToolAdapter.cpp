// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxManualToolAdapter.h"
#include "sscToolManager.h"

namespace cx
{

ManualToolAdapter::ManualToolAdapter(ssc::ToolManager* manager, QString uid) :
				ssc::ManualTool(manager, uid)
{
	mBase.reset(new ssc::ManualTool(manager, uid + "base"));
	connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
}

ManualToolAdapter::ManualToolAdapter(ssc::ToolManager* manager, ssc::ToolPtr base) :
				ssc::ManualTool(manager, mBase->getUid() + "_manual"), mBase(base)
{
}

ManualToolAdapter::~ManualToolAdapter()
{
}

void ManualToolAdapter::setBase(ssc::ToolPtr base)
{
	disconnect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
	mBase = base;
	connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
}

//ssc::Tool::Type ManualToolAdapter::getType() const
//{
//  return mBase->getType();
//}

QString ManualToolAdapter::getGraphicsFileName() const
{
	return mBase->getGraphicsFileName();
}

vtkPolyDataPtr ManualToolAdapter::getGraphicsPolyData() const
{
	return mBase->getGraphicsPolyData();
}

//QString ManualToolAdapter::getUid() const
//{
//  return mBase->getUid();
//}
//
//QString ManualToolAdapter::getName() const
//{
//  return mBase->getName();
//}

bool ManualToolAdapter::isCalibrated() const
{
	return mBase->isCalibrated();
}

ssc::ProbeData ManualToolAdapter::getProbeSector() const
{
	return mBase->getProbeSector();
}

ssc::ProbePtr ManualToolAdapter::getProbe() const
{
	return mBase->getProbe();
}

ssc::Transform3D ManualToolAdapter::getCalibration_sMt() const
{
	return mBase->getCalibration_sMt();
}

std::map<int, ssc::Vector3D> ManualToolAdapter::getReferencePoints() const
{
	return mBase->getReferencePoints();
}

}
