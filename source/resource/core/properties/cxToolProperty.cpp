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

#include "cxToolProperty.h"

#include "cxTypeConversions.h"
#include "cxTrackingService.h"
#include "cxTool.h"

namespace cx
{


StringPropertyActiveTool::StringPropertyActiveTool(TrackingServicePtr trackingService)
{
	mTrackingService = trackingService;
  connect(mTrackingService.get(), SIGNAL(activeToolChanged(const QString&)), this, SIGNAL(changed()));
  connect(mTrackingService.get(), &TrackingService::stateChanged, this, &StringPropertyActiveTool::changed);
}

QString StringPropertyActiveTool::getDisplayName() const
{
  return "Active Tool";
}

bool StringPropertyActiveTool::setValue(const QString& value)
{
  ToolPtr newTool = mTrackingService->getTool(value);
  if (!newTool)
	  return false;
  if(newTool == mTrackingService->getActiveTool())
    return false;
  mTrackingService->setActiveTool(newTool->getUid());
  return true;
}

QString StringPropertyActiveTool::getValue() const
{
  if (!mTrackingService->getActiveTool())
    return "";
  return qstring_cast(mTrackingService->getActiveTool()->getUid());
}

QString StringPropertyActiveTool::getHelp() const
{
  return "select the active tool";
}

QStringList StringPropertyActiveTool::getValueRange() const
{
	TrackingService::ToolMap tools = mTrackingService->getTools();

	QStringList retval;
	for (TrackingService::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
		retval << iter->second->getUid();
	return retval;
}

QString StringPropertyActiveTool::convertInternal2Display(QString internal)
{
  ToolPtr tool = mTrackingService->getTool(internal);
  if (!tool)
    return "<no tool>";
  return qstring_cast(tool->getName());
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

StringPropertyActiveProbeConfiguration::StringPropertyActiveProbeConfiguration(TrackingServicePtr trackingService)
{
	mTrackingService = trackingService;
  connect(mTrackingService.get(), &TrackingService::activeToolChanged, this, &StringPropertyActiveProbeConfiguration::activeToolChanged);
  connect(mTrackingService.get(), &TrackingService::stateChanged, this, &StringPropertyActiveProbeConfiguration::activeToolChanged);
  this->activeToolChanged();
}

void StringPropertyActiveProbeConfiguration::activeToolChanged()
{
	// ignore tool changes to something non-probeish.
	// This gives the user a chance to use the widget without having to show the probe.
	ToolPtr newTool = mTrackingService->getFirstProbe();
	if (!newTool || !newTool->getProbe())
		return;

	if (mTool)
		disconnect(mTool->getProbe().get(), &Probe::sectorChanged, this, &StringPropertyActiveProbeConfiguration::changed);

	mTool = newTool;

	if (mTool)
		connect(mTool->getProbe().get(), &Probe::sectorChanged, this, &StringPropertyActiveProbeConfiguration::changed);

	emit changed();
}

QString StringPropertyActiveProbeConfiguration::getDisplayName() const
{
  return "Probe Config";
}

bool StringPropertyActiveProbeConfiguration::setValue(const QString& value)
{
  if (!mTool)
    return false;
  mTool->getProbe()->applyNewConfigurationWithId(value);
  return true;
}

QString StringPropertyActiveProbeConfiguration::getValue() const
{
  if (!mTool)
    return "";
  return mTool->getProbe()->getConfigId();
}

QString StringPropertyActiveProbeConfiguration::getHelp() const
{
  return "Select a probe configuration for the active probe.";
}

QStringList StringPropertyActiveProbeConfiguration::getValueRange() const
{
  if (!mTool)
    return QStringList();
  return mTool->getProbe()->getConfigIdList();
}

QString StringPropertyActiveProbeConfiguration::convertInternal2Display(QString internal)
{
  if (!mTool)
    return "<no tool>";
  return mTool->getProbe()->getConfigName(internal); ///< get a name for the given configuration
}


}
