/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingSystemService.h"

namespace cx
{

TrackingSystemService::TrackingSystemService() :
	mState(Tool::tsNONE),
	mConfigurationFilePath(""),
	mLoggingFolder("")
{
}

Tool::State TrackingSystemService::getState() const
{
	return mState;
}

bool TrackingSystemService::isConfigured() const
{
	return mState>=Tool::tsCONFIGURED;
}

bool TrackingSystemService::isInitialized() const
{
	return mState>=Tool::tsINITIALIZED;
}

bool TrackingSystemService::isTracking() const
{
	return mState>=Tool::tsTRACKING;
}

void TrackingSystemService::setConfigurationFile(QString configurationFile)
{
	if (configurationFile == mConfigurationFilePath)
		return;

	if (this->isConfigured())
	{
		this->deconfigure();
	}

	mConfigurationFilePath = configurationFile;
}

void TrackingSystemService::setLoggingFolder(QString loggingFolder)
{
	if (mLoggingFolder == loggingFolder)
		return;

	if (this->isConfigured())
	{
		this->deconfigure();
	}

	mLoggingFolder = loggingFolder;
}

void TrackingSystemService::internalSetState(Tool::State val)
{
	if (mState==val)
		return;

	if (val > mState) // up
	{
		if (val == Tool::tsTRACKING)
			this->startTracking();
		else if (val == Tool::tsINITIALIZED)
			this->initialize();
		else if (val == Tool::tsCONFIGURED)
			this->configure();
	}
	else // down
	{
		if (val == Tool::tsINITIALIZED)
			this->stopTracking();
		else if (val == Tool::tsCONFIGURED)
			this->uninitialize();
		else if (val == Tool::tsNONE)
			this->deconfigure();
	}
}

void TrackingSystemService::initialize()
{
	if(!isConfigured())
		this->configure();
}

void TrackingSystemService::uninitialize()
{
}
void TrackingSystemService::startTracking()
{
	if(!isInitialized())
		this->initialize();
}

void TrackingSystemService::stopTracking()
{
}
}//cx
