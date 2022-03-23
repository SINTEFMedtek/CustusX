/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKERCONFIGURATION_H
#define CXTRACKERCONFIGURATION_H

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QStringList>

#include "cxToolConfigurationParser.h"

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
		QString mTrackingSystemName;//Tracking system name (polaris, aurora, ...)
		QStringList mTools;
		QString mReferenceTool;
		QString mTrackingSystemImplementation;//Tracking system implementation (igstk or openigtlink)
		bool mApplyRefToTools;//Apply ref position to all tools for OpenIGTLink transforms
		std::vector<cx::ConfigurationFileParser::ToolStructure> mToolList;
	};
	struct Tool
	{
		QString mUid; // absolute filename
		QString mName; // User-friendly name
		QString mTrackingSystemName;
		QString mPictureFilename;
		bool mIsReference;
		int mPortNumber;
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

	virtual QString getTrackingSystemImplementation() = 0;
	virtual void setTrackingSystemImplementation(QString trackingSystemImplementation) = 0;

	virtual bool isNull() = 0;
	static TrackerConfigurationPtr getNullObject();

protected:
	QString mTrackingSystemImplementation;
};

} // namespace cx

#endif // CXTRACKERCONFIGURATION_H
