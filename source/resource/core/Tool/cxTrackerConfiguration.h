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

#ifndef CXTRACKERCONFIGURATION_H
#define CXTRACKERCONFIGURATION_H

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QStringList>

namespace cx
{
typedef boost::shared_ptr<class TrackerConfiguration> TrackerConfigurationPtr;

/** Tracker and Tool configuration info
 *
 * \date 2014-06-15
 * \author christiana
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT TrackerConfiguration
{
public:
	struct Configuration
	{
		QString mUid; ///< absolute path and filename for the new config file
		QString mName;
		QString mClinicalApplication;
		QString mTrackingSystemName; //Tracking system name. Converted to and from emun. See string list in cxDefinitionStrings.cpp. Enums defined in cxDefinitions.h
		QStringList mTools;
		QString mReferenceTool;
	};
	struct Tool
	{
		QString mUid; // absolute filename
		QString mName; // User-friendly name
		QString mTrackingSystemName;
		QString mPictureFilename;
		bool mIsReference;
	};

	virtual ~TrackerConfiguration();

	virtual QString getConfigurationApplicationsPath() = 0;
	virtual void saveConfiguration(const Configuration& config) = 0;
	virtual Configuration getConfiguration(QString uid) = 0;

	virtual QStringList getConfigurationsGivenApplication() = 0;
	virtual QStringList getAllConfigurations() = 0;

	virtual QStringList getSupportedTrackingSystems() = 0;
	virtual QStringList getToolsGivenFilter(QStringList applicationsFilter,
											QStringList trackingsystemsFilter) = 0;
	virtual QStringList getAllTools() = 0;
	virtual QStringList getAllApplications() = 0;

	virtual Tool getTool(QString uid) = 0;
	virtual bool verifyTool(QString uid) = 0;

	virtual bool isNull() = 0;
	static TrackerConfigurationPtr getNullObject();
};

} // namespace cx

#endif // CXTRACKERCONFIGURATION_H
