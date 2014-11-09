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

#include "cxTrackingServiceNull.h"
#include "cxReporter.h"
#include "cxTrackerConfiguration.h"


namespace cx
{
TrackingServiceNull::TrackingServiceNull()
{
}
bool TrackingServiceNull::isNull()
{
	return true;
}

void TrackingServiceNull::printWarning() const
{
	reportWarning("Trying to use TrackingServiceNull. Is TrackingService (org.custusx.core.tracking) disabled?");
}

Tool::State TrackingServiceNull::getState() const
{
	return Tool::tsNONE;
}

void TrackingServiceNull::setState(const Tool::State val)
{

}

ToolPtr TrackingServiceNull::getTool(const QString& uid)
{
	this->printWarning();
	return ToolPtr();
}

ToolPtr TrackingServiceNull::getActiveTool()
{
	this->printWarning();
	return ToolPtr();
}

void TrackingServiceNull::setActiveTool(const QString& uid)
{
	this->printWarning();
}

ToolPtr TrackingServiceNull::getFirstProbe()
{
	this->printWarning();
	return ToolPtr();
}

ToolPtr TrackingServiceNull::getReferenceTool() const
{
	this->printWarning();
	return ToolPtr();
}

ToolPtr TrackingServiceNull::getManualTool()
{
	this->printWarning();
	return ToolPtr();
}

TrackingService::ToolMap TrackingServiceNull::getTools()
{
	return ToolMap();
}

bool TrackingServiceNull::isPlaybackMode() const
{
	return false;
}

void TrackingServiceNull::setPlaybackMode(PlaybackTimePtr controller)
{

}

void TrackingServiceNull::savePositionHistory()
{

}

void TrackingServiceNull::loadPositionHistory()
{

}

void TrackingServiceNull::addXml(QDomNode& parentNode)
{

}

void TrackingServiceNull::parseXml(QDomNode& dataNode)
{

}

void TrackingServiceNull::clear()
{

}

SessionToolHistoryMap TrackingServiceNull::getSessionHistory(double startTime, double stopTime)
{
	return SessionToolHistoryMap();
}

void TrackingServiceNull::setLoggingFolder(QString loggingFolder)
{

}

void TrackingServiceNull::runDummyTool(DummyToolPtr tool)
{

}


TrackerConfigurationPtr TrackingServiceNull::getConfiguration()
{
	return TrackerConfiguration::getNullObject();
}

} //cx
