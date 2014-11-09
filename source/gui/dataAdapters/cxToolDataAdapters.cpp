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

#include <cxToolDataAdapters.h>

#include "cxTypeConversions.h"
#include "cxToolManager.h"
#include "cxTool.h"

namespace cx
{


ActiveToolStringDataAdapter::ActiveToolStringDataAdapter()
{
  connect(trackingService().get(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
  connect(trackingService().get(), &ToolManager::stateChanged, this, &ActiveToolStringDataAdapter::changed);
}

QString ActiveToolStringDataAdapter::getDisplayName() const
{
  return "Active Tool";
}

bool ActiveToolStringDataAdapter::setValue(const QString& value)
{
  ToolPtr newTool = trackingService()->getTool(value);
  if (!newTool)
	  return false;
  if(newTool == trackingService()->getActiveTool())
    return false;
  trackingService()->setActiveTool(newTool->getUid());
  return true;
}

QString ActiveToolStringDataAdapter::getValue() const
{
  if (!trackingService()->getActiveTool())
    return "";
  return qstring_cast(trackingService()->getActiveTool()->getUid());
}

QString ActiveToolStringDataAdapter::getHelp() const
{
  return "select the active (dominant) tool";
}

QStringList ActiveToolStringDataAdapter::getValueRange() const
{
	ToolManager::ToolMap tools = trackingService()->getTools();

	QStringList retval;
	for (ToolManager::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
		retval << iter->second->getUid();
	return retval;
}

QString ActiveToolStringDataAdapter::convertInternal2Display(QString internal)
{
  ToolPtr tool = trackingService()->getTool(internal);
  if (!tool)
    return "<no tool>";
  return qstring_cast(tool->getName());
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------



/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


ActiveProbeConfigurationStringDataAdapter::ActiveProbeConfigurationStringDataAdapter()
{
  connect(trackingService().get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
  connect(trackingService().get(), &ToolManager::stateChanged, this, &ActiveProbeConfigurationStringDataAdapter::dominantToolChanged);
  this->dominantToolChanged();
}

void ActiveProbeConfigurationStringDataAdapter::dominantToolChanged()
{
	// ignore tool changes to something non-probeish.
	// This gives the user a chance to use the widget without having to show the probe.
	ToolPtr newTool = trackingService()->findFirstProbe();
	if (!newTool || !newTool->getProbe())
		return;

	if (mTool)
		disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

	mTool = newTool;

	if (mTool)
		connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

	emit changed();
}

QString ActiveProbeConfigurationStringDataAdapter::getDisplayName() const
{
  return "Probe Config";
}

bool ActiveProbeConfigurationStringDataAdapter::setValue(const QString& value)
{
  if (!mTool)
    return false;
  mTool->getProbe()->applyNewConfigurationWithId(value);
  return true;
}

QString ActiveProbeConfigurationStringDataAdapter::getValue() const
{
  if (!mTool)
    return "";
  return mTool->getProbe()->getConfigId();
}

QString ActiveProbeConfigurationStringDataAdapter::getHelp() const
{
  return "Select a probe configuration for the active probe.";
}

QStringList ActiveProbeConfigurationStringDataAdapter::getValueRange() const
{
  if (!mTool)
    return QStringList();
  return mTool->getProbe()->getConfigIdList();
}

QString ActiveProbeConfigurationStringDataAdapter::convertInternal2Display(QString internal)
{
  if (!mTool)
    return "<no tool>";
  return mTool->getProbe()->getConfigName(internal); ///< get a name for the given configuration
}


}
