/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxManualToolAdapter.h"
#include <QTimer>

namespace cx
{

ManualToolAdapter::ManualToolAdapter(QString uid) :
	ManualTool(uid)
{
	mInitialBase = ToolPtr(new ManualTool(uid + "base"));
	this->setBase(mInitialBase);
	//	mBase.reset(new ManualTool(uid + "base"));
	//	connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
}

ManualToolAdapter::ManualToolAdapter(ToolPtr base) :
	ManualTool(mBase->getUid() + "_manual"), mBase(base)
{
}

ManualToolAdapter::~ManualToolAdapter()
{
}

void ManualToolAdapter::setBase(ToolPtr base)
{
	if (mBase)
	{
		disconnect(mBase.get(), &Tool::toolTransformAndTimestamp, this, &Tool::toolTransformAndTimestamp);
		disconnect(mBase.get(), &Tool::toolVisible, this, &Tool::toolVisible);
		disconnect(mBase.get(), &Tool::tooltipOffset, this, &Tool::tooltipOffset);
		disconnect(mBase.get(), &Tool::toolProbeSector, this, &Tool::toolProbeSector);
		disconnect(mBase.get(), &Tool::tps, this, &Tool::tps);
	}

	mBase = base;

	if(!mBase)
		mBase = mInitialBase;

	if (mBase)
	{
		connect(mBase.get(), &Tool::toolTransformAndTimestamp, this, &Tool::toolTransformAndTimestamp);
		connect(mBase.get(), &Tool::toolVisible, this, &Tool::toolVisible);
		connect(mBase.get(), &Tool::tooltipOffset, this, &Tool::tooltipOffset);
		connect(mBase.get(), &Tool::toolProbeSector, this, &Tool::toolProbeSector);
		connect(mBase.get(), &Tool::tps, this, &Tool::tps);
	}

	emit toolVisible(this->getVisible());
	emit toolTransformAndTimestamp(this->get_prMt(), this->getTimestamp());
	emit tooltipOffset(this->getTooltipOffset());
	emit toolProbeSector();
	emit tps(0);
}

vtkPolyDataPtr ManualToolAdapter::getGraphicsPolyData() const
{
	return mBase->getGraphicsPolyData();
}

bool ManualToolAdapter::isCalibrated() const
{
	return mBase->isCalibrated();
}

ProbePtr ManualToolAdapter::getProbe() const
{
	return mBase->getProbe();
}

Transform3D ManualToolAdapter::getCalibration_sMt() const
{
	return mBase->getCalibration_sMt();
}

std::map<QString, Vector3D> ManualToolAdapter::getReferencePoints() const
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

std::set<Tool::Type> ManualToolAdapter::getTypes() const
{
	std::set<Tool::Type> retval = mBase->getTypes();
	retval.insert(Tool::TOOL_MANUAL);
	return retval;
}

void ManualToolAdapter::startEmittingContinuousPositions(int msecBetweenPositions)
{
	if(!mPositionTimer)
		mPositionTimer = new QTimer(this);
	connect(mPositionTimer, &QTimer::timeout, this, &ManualToolAdapter::emitPosition);
	mPositionTimer->start(msecBetweenPositions);
}

void ManualToolAdapter::stopEmittingContinuousPositions()
{
	if(mPositionTimer)
	{
		mPositionTimer->stop();
		disconnect(mPositionTimer, &QTimer::timeout, this, &ManualToolAdapter::emitPosition);
	}
}

void ManualToolAdapter::emitPosition()
{
	emit toolTransformAndTimestamp(m_prMt, -1);
}

}
