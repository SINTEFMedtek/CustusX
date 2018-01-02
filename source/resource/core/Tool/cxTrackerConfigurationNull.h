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
#ifndef CXTRACKERCONFIGURATIONNULL_H
#define CXTRACKERCONFIGURATIONNULL_H

#include "cxResourceExport.h"

#include "cxTrackerConfiguration.h"

namespace cx
{
class cxResource_EXPORT TrackerConfigurationNull : public TrackerConfiguration
{
public:
	TrackerConfigurationNull() {}

	virtual QString getConfigurationApplicationsPath() { return ""; }
	virtual void saveConfiguration(const Configuration& config) {}
	virtual Configuration getConfiguration(QString uid) { return Configuration(); }

	virtual QStringList getConfigurationsGivenApplication() { return QStringList(); }
	virtual QStringList getAllConfigurations() { return QStringList(); }

	virtual QStringList getSupportedTrackingSystems() { return QStringList(); }
	virtual QStringList getToolsGivenFilter(QStringList applicationsFilter,
											QStringList trackingsystemsFilter) { return QStringList(); }
	virtual QStringList getAllTools() { return QStringList(); }
    virtual QStringList getAllApplications() { return QStringList(); }

	virtual Tool getTool(QString uid) { return Tool(); }
	virtual bool verifyTool(QString uid) { return false; }

	virtual QString getTrackingSystemImplementation() {return QString();}
	virtual void setTrackingSystemImplementation(QString trackingSystemSolution) {}

	virtual bool isNull() { return true; }
};
} //cx

#endif // CXTRACKERCONFIGURATIONNULL_H
