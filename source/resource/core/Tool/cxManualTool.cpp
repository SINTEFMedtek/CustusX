/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
