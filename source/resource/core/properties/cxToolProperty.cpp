/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
