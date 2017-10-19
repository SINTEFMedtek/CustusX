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


#include "cxManualTool.h"
#include <QTime>
#include <vtkSTLReader.h>
#include <vtkCursor3D.h>
#include "cxDummyTool.h"

#include "cxTime.h"

namespace cx
{

ManualTool::ManualTool(const QString& uid, const QString& name) :
	ToolImpl(uid, name)
{
	mTimestamp = 0;
	mVisible = false;
//	read3DCrossHairSlot(0);
	connect(this, SIGNAL(tooltipOffset(double)), this, SLOT(read3DCrossHairSlot(double)));
}

ManualTool::~ManualTool()
{
}

void ManualTool::read3DCrossHairSlot(double toolTipOffset) const
{
	if (!mCrossHair)
		return;
	this->update3DCrossHair(toolTipOffset);
}

void ManualTool::update3DCrossHair(double toolTipOffset) const
{
	if (!mCrossHair)
	{
		mCrossHair = vtkCursor3DPtr::New();
		mCrossHair->AllOff();
		mCrossHair->AxesOn();
	}
	int s = 60;
	mCrossHair->SetModelBounds(-s, s, -s, s, -s, s + toolTipOffset);
	mCrossHair->SetFocalPoint(0, 0, toolTipOffset);
	mCrossHair->Update();
}


/**Set tool position and timestamp
 */
void ManualTool::set_prMt(const Transform3D& prMt, double timestamp)
{
	if (timestamp < 0)
		timestamp = getMilliSecondsSinceEpoch();
	mTimestamp = timestamp;
	ToolImpl::set_prMt(prMt, timestamp);
}

std::set<Tool::Type> ManualTool::getTypes() const
{
	std::set<Type> retval;
	retval.insert(Tool::TOOL_MANUAL);
	return retval;
}


vtkPolyDataPtr ManualTool::getGraphicsPolyData() const
{
	this->update3DCrossHair(this->getTooltipOffset());
	return mCrossHair->GetOutput();
}

bool ManualTool::getVisible() const
{
	return mVisible;
}

int ManualTool::getIndex() const
{
	return -1;
}

QString ManualTool::getUid() const
{
	return mUid;
}

QString ManualTool::getName() const
{
	return mName;
}

void ManualTool::setVisible(bool vis)
{
	if (mVisible==vis)
	  return;
	mVisible = vis;
	emit toolVisible(mVisible);
}

bool ManualTool::isCalibrated() const
{
	return false;
}

double ManualTool::getTimestamp() const
{
	return mTimestamp;
}

Transform3D ManualTool::getCalibration_sMt() const
{
	return Transform3D::Identity();
}

}//end namespace
