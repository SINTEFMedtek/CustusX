/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
