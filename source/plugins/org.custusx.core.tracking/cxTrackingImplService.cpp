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

#include "cxTrackingImplService.h"

#include <ctkPluginContext.h>
#include "cxData.h"
#include "cxReporter.h"
#include "cxLogicManager.h"
#include "cxDataManager.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxRegistrationTransform.h"
#include "cxToolManager.h"

namespace cx
{

TrackingImplService::TrackingImplService(ctkPluginContext *context) :
	mContext(context )
{
//	mOld = LogicManager::getInstance()->getTrackingService();
	connect(mOld.get(), &ToolManager::stateChanged, this, &TrackingService::stateChanged);
	connect(mOld.get(), &ToolManager::dominantToolChanged, this, &TrackingService::dominantToolChanged);
}

TrackingImplService::~TrackingImplService()
{
}

TrackingServiceOldPtr TrackingImplService::getOld() const
{
	return mOld;
}


bool TrackingImplService::isNull()
{
	return false;
}

Tool::State TrackingImplService::getState() const
{
	return this->getOld()->getState();
}

void TrackingImplService::setState(const Tool::State val)
{
	this->getOld()->setState(val);
}

ToolPtr TrackingImplService::getTool(const QString& uid)
{
	return this->getOld()->getTool(uid);
}

ToolPtr TrackingImplService::getActiveTool()
{
	return this->getOld()->getActiveTool();
}

void TrackingImplService::setActiveTool(const QString& uid)
{
	this->getOld()->setActiveTool(uid);
}

ToolPtr TrackingImplService::getFirstProbe()
{
	return this->getOld()->getFirstProbe();
}

ToolPtr TrackingImplService::getReferenceTool() const
{
	return this->getOld()->getReferenceTool();
}

ToolPtr TrackingImplService::getManualTool()
{
	return this->getOld()->getManualTool();
}






TrackingService::ToolMap TrackingImplService::getTools()
{
	return this->getOld()->getTools();
}

bool TrackingImplService::isPlaybackMode() const
{
	return this->getOld()->isPlaybackMode();
}

void TrackingImplService::setPlaybackMode(PlaybackTimePtr controller)
{
	this->getOld()->setPlaybackMode(controller);
}

void TrackingImplService::savePositionHistory()
{
	this->getOld()->savePositionHistory();
}

void TrackingImplService::loadPositionHistory()
{
	this->getOld()->loadPositionHistory();
}

void TrackingImplService::addXml(QDomNode& parentNode)
{
	this->getOld()->addXml(parentNode);
}

void TrackingImplService::parseXml(QDomNode& dataNode)
{
	this->getOld()->parseXml(dataNode);
}

void TrackingImplService::clear()
{
	this->getOld()->clear();
}

SessionToolHistoryMap TrackingImplService::getSessionHistory(double startTime, double stopTime)
{
	return this->getOld()->getSessionHistory(startTime, stopTime);
}

void TrackingImplService::setLoggingFolder(QString loggingFolder)
{
	this->getOld()->setLoggingFolder(loggingFolder);
}

void TrackingImplService::runDummyTool(DummyToolPtr tool)
{
	this->getOld()->runDummyTool(tool);
}

QStringList TrackingImplService::getSupportedTrackingSystems()
{
	return this->getOld()->getSupportedTrackingSystems();
}

TrackerConfigurationPtr TrackingImplService::getConfiguration()
{
	return this->getOld()->getConfiguration();
}



} /* namespace cx */
