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
		mActiveToolSelected = true;
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
