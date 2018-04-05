/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTRACKERCONFIGURATIONIMPL_H
#define CXTRACKERCONFIGURATIONIMPL_H

#include "cxResourceExport.h"

#include <QDir>
#include "cxTrackerConfiguration.h"
#include "cxToolFileParser.h"

namespace cx
{

/**
 * \ingroup org_custusx_core_tracking
 */
class cxResource_EXPORT TrackerConfigurationImpl : public TrackerConfiguration
{
public:
	virtual ~TrackerConfigurationImpl() {}

	virtual void saveConfiguration(const Configuration& config);
	virtual Configuration getConfiguration(QString uid);

	virtual QStringList getSupportedTrackingSystems();
	virtual QStringList getToolsGivenFilter(QStringList applicationsFilter,
											QStringList trackingsystemsFilter);
	virtual Tool getTool(QString uid);
	virtual bool verifyTool(QString uid);

	virtual QString getConfigurationApplicationsPath();
	virtual QStringList getConfigurationsGivenApplication();
	virtual QStringList getAllConfigurations();
	virtual QStringList getAllTools();
	virtual QStringList getAllApplications();

	virtual QString getTrackingSystemImplementation();
	virtual void setTrackingSystemImplementation(QString trackingSystemImplementation);

	virtual bool isNull() { return false; }

private:
	QStringList filter(QStringList toolsToFilter, QStringList applicationsFilter,
			QStringList trackingsystemsFilter);
		ToolFileParser::ToolInternalStructurePtr getToolInternal(QString toolAbsoluteFilePath);

};


} // namespace cx



#endif // CXTRACKERCONFIGURATIONIMPL_H
