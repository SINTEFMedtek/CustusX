/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStringPropertySelectTool.h"

#include "cxTrackingService.h"
#include "cxTypeConversions.h"

namespace cx
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertySelectTool::StringPropertySelectTool(TrackingServicePtr trackingService) :
	mProvideActiveTool(false),
	mActiveToolSelected(false),
	mActiveToolName("<Active Tool>")
{
	mTrackingService = trackingService;
	connect(mTrackingService.get(), &TrackingService::stateChanged, this, &Property::changed);
	mValueName = "Select a tool";
	mHelp = mValueName;
}

void StringPropertySelectTool::setHelp(QString help)
{
  mHelp = help;
}

void StringPropertySelectTool::setValueName(QString name)
{
  mValueName = name;
}

QString StringPropertySelectTool::getDisplayName() const
{
  return mValueName;
}

bool StringPropertySelectTool::setValue(const QString& value)
{
	if(value == mActiveToolName)
	{
		mTool.reset(); //Don't remember last tool when active tool is selected
		bool emitChange = false;
		if(!mActiveToolSelected)
			emitChange = true;
		mActiveToolSelected = true;
		if(emitChange)
			emit changed();
		return true;
	}
	else
		mActiveToolSelected = false;

  if(mTool && value==mTool->getUid())
	return false;
  ToolPtr temp = mTrackingService->getTool(value);
  if(!temp)
	return false;

  mTool = temp;
  emit changed();
  return true;
}

QString StringPropertySelectTool::getValue() const
{
	if(mActiveToolSelected)
		return mActiveToolName;
  if(!mTool)
	return "<no tool>";
  return mTool->getUid();
}

QString StringPropertySelectTool::getHelp() const
{
  return mHelp;
}

ToolPtr StringPropertySelectTool::getTool() const
{
	if(mActiveToolSelected)
		return mTrackingService->getActiveTool();
  return mTool;
}

bool StringPropertySelectTool::isActiveToolSelected() const
{
	return mActiveToolSelected;
}

QStringList StringPropertySelectTool::getValueRange() const
{
	TrackingService::ToolMap tools = mTrackingService->getTools();

	QStringList retval;

	if(mProvideActiveTool)
		retval << mActiveToolName;

	for (TrackingService::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
		retval << iter->second->getUid();
	return retval;
}

QString StringPropertySelectTool::convertInternal2Display(QString internal)
{
	if(internal == mActiveToolName)
		return mActiveToolName;
  ToolPtr tool = mTrackingService->getTool(internal);
  if (!tool)
  {
	return "<no tool>";
  }
  return qstring_cast(tool->getName());
}

void StringPropertySelectTool::provideActiveTool(bool on)
{
	mProvideActiveTool = on;
}

void StringPropertySelectTool::setActiveTool()
{
	mActiveToolSelected = true;
}

} // namespace cx
