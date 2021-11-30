/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxBronchoscopyTool.h"
#include "cxTypeConversions.h"
#include "cxBronchoscopePositionProjection.h"
#include "cxDoubleProperty.h"
#include "cxLogger.h"

namespace cx
{

BronchoscopyTool::BronchoscopyTool(ToolPtr base, BronchoscopePositionProjectionPtr projectionCenterline) :
	ToolImpl(base->getUid(), "Bronchoscopy Navigation "+base->getName()),
	mBase(base),
	mProjectionCenterline(projectionCenterline)
{
	connect(mBase.get(), &Tool::toolProbeSector, this, &Tool::toolProbeSector);
	connect(mBase.get(), &Tool::tooltipOffset, this, &Tool::tooltipOffset);
	connect(mBase.get(), &Tool::toolTransformAndTimestamp, this, &BronchoscopyTool::onToolTransformAndTimestamp);
	connect(mBase.get(), &Tool::toolVisible, this, &Tool::toolVisible);
	connect(mBase.get(), &Tool::tooltipOffset, this, &Tool::tooltipOffset);
	connect(mBase.get(), &Tool::tps, this, &Tool::tps);
}

BronchoscopyTool::~BronchoscopyTool()
{
}


std::set<Tool::Type> BronchoscopyTool::getTypes() const
{
	return mBase->getTypes();
}

void BronchoscopyTool::onToolTransformAndTimestamp(Transform3D prMt, double timestamp)
{
	double maxDistanceToCenterline = mProjectionCenterline->getMaxDistanceToCenterlineValue();
	double maxSearchDistance = mProjectionCenterline->getMaxSearchDistanceValue();

	if (mProjectionCenterline->isAdvancedCenterlineProjectionSelected())
		m_prMt = mProjectionCenterline->findProjectedPoint(prMt, maxDistanceToCenterline, maxSearchDistance);
	else
		m_prMt = mProjectionCenterline->findClosestPointInBranches(prMt,maxDistanceToCenterline);

	emit toolTransformAndTimestamp(m_prMt, timestamp);
}

vtkPolyDataPtr BronchoscopyTool::getGraphicsPolyData() const
{
	return mBase->getGraphicsPolyData();
}

Transform3D BronchoscopyTool::get_prMt() const
{
	return m_prMt;
}

bool BronchoscopyTool::getVisible() const
{
	return mBase->getVisible();
}

QString BronchoscopyTool::getUid() const
{
	return mUid;
}

QString BronchoscopyTool::getName() const
{
	return mName;
}

bool BronchoscopyTool::isCalibrated() const
{
	return mBase->isCalibrated();
}

double BronchoscopyTool::getTimestamp() const
{
	return mBase->getTimestamp();
}

// Just use the tool tip offset from the tool manager
double BronchoscopyTool::getTooltipOffset() const
{
	return mBase->getTooltipOffset();
}

// Just use the tool tip offset from the tool manager
void BronchoscopyTool::setTooltipOffset(double val)
{
	mBase->setTooltipOffset(val);
}

Transform3D BronchoscopyTool::getCalibration_sMt() const
{
	return mBase->getCalibration_sMt();
}

std::map<QString, Vector3D> BronchoscopyTool::getReferencePoints() const
{
	return mBase->getReferencePoints();
}

bool BronchoscopyTool::isInitialized() const
{
	return mBase->isInitialized();
}

void BronchoscopyTool::set_prMt(const Transform3D& prMt, double timestamp)
{
	mBase->set_prMt(prMt, timestamp);

}

void BronchoscopyTool::setVisible(bool vis)
{
	mBase->setVisible(vis);
}


} /* namespace cx */
