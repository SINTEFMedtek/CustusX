/*
 * cxManualToolAdapter.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: christiana
 */

#include <cxManualToolAdapter.h>
#include "sscToolManager.h"

namespace cx
{

ManualToolAdapter::ManualToolAdapter(ssc::ToolManager* manager, QString uid) : ssc::ManualTool(manager, uid)
{
  mBase.reset(new ssc::ManualTool(manager, uid+"base"));
  connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
}

ManualToolAdapter::ManualToolAdapter(ssc::ToolManager* manager, ssc::ToolPtr base) : ssc::ManualTool(manager, mBase->getUid()+"_manual"), mBase(base)
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
