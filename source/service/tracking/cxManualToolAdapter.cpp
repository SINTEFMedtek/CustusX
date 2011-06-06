/*
 * cxManualToolAdapter.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: christiana
 */

#include <cxManualToolAdapter.h>

namespace cx
{

ManualToolAdapter::ManualToolAdapter(QString uid) : ssc::ManualTool(uid)
{
  mBase.reset(new ssc::ManualTool(uid+"base"));
  connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
}

ManualToolAdapter::ManualToolAdapter(ssc::ToolPtr base) : ssc::ManualTool(mBase->getUid()+"_manual"), mBase(base)
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

ssc::Tool::Type ManualToolAdapter::getType() const
{
  return mBase->getType();
}

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
