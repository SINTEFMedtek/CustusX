// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscManualTool.h"
#include <QTime>
#include <vtkSTLReader.h>
#include <vtkCursor3D.h>
#include "sscDummyTool.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscTime.h"

namespace ssc
{

ManualTool::ManualTool(ToolManager* manager, const QString& uid, const QString& name) :
	Tool(uid,name), mMutex(QMutex::Recursive)
{
	mTimestamp = 0;
	m_prMt = Transform3D::Identity();
	mVisible = false;
	read3DCrossHairSlot(0);
	connect(manager, SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
	connect(manager, SIGNAL(tooltipOffset(double)), this, SLOT(read3DCrossHairSlot(double)));
}

ManualTool::~ManualTool()
{
}

void ManualTool::read3DCrossHairSlot(double toolTipOffset)
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
	mCrossHair->Modified();
}

/**Set tool position, use current time as timestamp
 */
void ManualTool::set_prMt(const Transform3D& prMt)
{
	this->set_prMt(prMt, ssc::getMilliSecondsSinceEpoch());
}

/**Set tool position and timestamp
 */
void ManualTool::set_prMt(const Transform3D& prMt, double timestamp)
{
	QMutexLocker locker(&mMutex);
	mTimestamp = timestamp;
	m_prMt = prMt;
	locker.unlock();

	emit toolTransformAndTimestamp(prMt, mTimestamp);

}

QString ManualTool::getGraphicsFileName() const
{
	return "";
}

std::set<Tool::Type> ManualTool::getTypes() const
{
	std::set<Type> retval;
	retval.insert(Tool::TOOL_MANUAL);
	return retval;
}


vtkPolyDataPtr ManualTool::getGraphicsPolyData() const
{
	return mCrossHair->GetOutput();
}

Transform3D ManualTool::get_prMt() const
{
	QMutexLocker locker(&mMutex);
	return m_prMt;
}

bool ManualTool::getVisible() const
{
	QMutexLocker locker(&mMutex);
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
	QMutexLocker locker(&mMutex);
	if (mVisible==vis)
	  return;
	mVisible = vis;
	emit toolVisible(mVisible);
}

bool ManualTool::isCalibrated() const
{
	return false;
}

ssc::ProbeData ManualTool::getProbeSector() const
{
	return mSector;
}

double ManualTool::getTimestamp() const
{
	return mTimestamp;
}

// Just use the tool tip offset from the tool manager
double ManualTool::getTooltipOffset() const
{
	return toolManager()->getTooltipOffset();
}

// Just use the tool tip offset from the tool manager
void ManualTool::setTooltipOffset(double val)
{
	toolManager()->setTooltipOffset(val);
}

ssc::Transform3D ManualTool::getCalibration_sMt() const
{
	return Transform3D::Identity();
}

}//end namespace
