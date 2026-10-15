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
#include "cxTrackingSystemBronchoscopyService.h"


#include "cxReporter.h"
#include "cxTime.h"
#include "cxPlaybackTime.h"
#include "cxBronchoscopyTool.h"
#include <algorithm>
#include <QApplication>
#include "cxUtilHelpers.h"
#include "cxManualTool.h"
#include "cxTrackerConfiguration.h"
#include "cxBronchoscopePositionProjection.h"


namespace cx
{

TrackingSystemBronchoscopyService::TrackingSystemBronchoscopyService(TrackingServicePtr trackingService, BronchoscopePositionProjectionPtr projectionCenterline, ToolPtr tool):
	mTrackingService(trackingService),
	mProjectionCenterline(projectionCenterline),
	mTool(tool)
{
	if (!mTool)
		mTool = mTrackingService->getActiveTool(); // Use active tool if tool is not set

	this->setBase(findTrackingSystemWithTool(mTrackingService->getTrackingSystems(), mTool));
	if (!mBase)
		reportWarning("Bronchoscopy navigation: Found no tracking system with the tool to lock to the centerline.");
}

TrackingSystemBronchoscopyService::~TrackingSystemBronchoscopyService()
{
}

bool TrackingSystemBronchoscopyService::setTrackingSystem(QString trackingSystemName)
{
	std::vector<TrackingSystemServicePtr> trackingSystems = mTrackingService->getTrackingSystems();
	for (int i=0; i<trackingSystems.size(); i++)
		if (trackingSystems[i]->getUid() == trackingSystemName)
		{
			this->setBase(trackingSystems[i]);
			return true;
		}
	return false;
}

TrackingSystemServicePtr TrackingSystemBronchoscopyService::findTrackingSystemWithTool(std::vector<TrackingSystemServicePtr> trackingSystems, ToolPtr tool)
{
	if (!tool)
		return TrackingSystemServicePtr();

	for (unsigned i = 0; i < trackingSystems.size(); ++i)
	{
		std::vector<ToolPtr> tools = trackingSystems[i]->getTools();
		if (std::find(tools.begin(), tools.end(), tool) != tools.end())
			return trackingSystems[i];
	}
	return TrackingSystemServicePtr();
}

void TrackingSystemBronchoscopyService::setBase(TrackingSystemServicePtr base)
{
	if (mBase)
		disconnect(mBase.get(), &TrackingSystemService::stateChanged, this, &TrackingSystemBronchoscopyService::onStateChanged);
	mBase = base;
	if (mBase)
		connect(mBase.get(), &TrackingSystemService::stateChanged, this, &TrackingSystemBronchoscopyService::onStateChanged);
	this->onStateChanged();
}

void TrackingSystemBronchoscopyService::setTool(ToolPtr tool)
{
	mTool = tool;
}

void TrackingSystemBronchoscopyService::onStateChanged()
{
	mTools.clear();
	if (!mBase)
	{
		emit stateChanged();
		return;
	}
	std::vector<ToolPtr> tools	 = mBase->getTools();
	for (unsigned i=0; i<tools.size(); ++i)
	{
		if (tools[i] == mTool)
		{
			BronchoscopyToolPtr current(new BronchoscopyTool(tools[i], mProjectionCenterline));
			mTools.push_back(current);
		}
		else
			mTools.push_back(tools[i]);
	}
	emit stateChanged();
}

ToolPtr TrackingSystemBronchoscopyService::getReference()
{
	return mBase->getReference();
}


std::vector<ToolPtr> TrackingSystemBronchoscopyService::getTools()
{
	return mTools;
}

Tool::State TrackingSystemBronchoscopyService::getState() const
{
	return mBase->getState();
}

void TrackingSystemBronchoscopyService::setState(const Tool::State val)
{
	mBase->setState(val);
}

void TrackingSystemBronchoscopyService::setLoggingFolder(QString loggingFolder)
{
	mBase->setLoggingFolder(loggingFolder);
}

TrackerConfigurationPtr TrackingSystemBronchoscopyService::getConfiguration()
{
	return mBase->getConfiguration();
}


} // namespace cx
