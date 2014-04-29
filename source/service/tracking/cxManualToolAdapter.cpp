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
#include "cxToolManager.h"

namespace cx
{

ManualToolAdapter::ManualToolAdapter(TrackingServicePtr manager, QString uid) :
				ManualTool(manager, uid)
{
	mBase.reset(new ManualTool(manager, uid + "base"));
	connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
}

ManualToolAdapter::ManualToolAdapter(TrackingServicePtr manager, ToolPtr base) :
				ManualTool(manager, mBase->getUid() + "_manual"), mBase(base)
{
}

ManualToolAdapter::~ManualToolAdapter()
{
}

void ManualToolAdapter::setBase(ToolPtr base)
{
	disconnect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
	mBase = base;
	connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));

	emit toolProbeSector();
}

vtkPolyDataPtr ManualToolAdapter::getGraphicsPolyData() const
{
	return mBase->getGraphicsPolyData();
}

bool ManualToolAdapter::isCalibrated() const
{
	return mBase->isCalibrated();
}

ProbeDefinition ManualToolAdapter::getProbeSector() const
{
	return mBase->getProbeSector();
}

ProbePtr ManualToolAdapter::getProbe() const
{
	return mBase->getProbe();
}

Transform3D ManualToolAdapter::getCalibration_sMt() const
{
	return mBase->getCalibration_sMt();
}

std::map<int, Vector3D> ManualToolAdapter::getReferencePoints() const
{
	return mBase->getReferencePoints();
}

double ManualToolAdapter::getTooltipOffset() const
{
	return mBase->getTooltipOffset();
}

void ManualToolAdapter::setTooltipOffset(double val)
{
	mBase->setTooltipOffset(val);
}

}
