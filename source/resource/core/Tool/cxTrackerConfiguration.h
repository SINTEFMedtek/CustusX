// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXTRACKERCONFIGURATION_H
#define CXTRACKERCONFIGURATION_H

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
class TrackerConfiguration
{
public:
	struct Configuration
	{
		QString mFileName; ///< absolute path and filename for the new config file
		QString mClinicalApplication;
		QString mTrackingSystem;
		QStringList mTools;
		QString mReferenceTool;
	};
	struct Tool
	{
		QString mUid; // absolute filename
		QString mName; // User-friendly name
		QString mTrackingSystem;
		QString mPictureFilename;
		bool mIsReference;
	};

	virtual ~TrackerConfiguration() {}

	virtual void saveConfiguration(const Configuration& config) = 0;
	virtual Configuration getConfiguration(QString uid) = 0;

	virtual QStringList getToolsGivenFilter(QStringList applicationsFilter,
											QStringList trackingsystemsFilter) = 0;
	virtual QString getToolName(QString uid) = 0;
	virtual QString getToolTrackingSystem(QString uid) = 0;
	virtual QString getToolPictureFilename(QString uid) = 0;
	virtual Tool getTool(QString uid) = 0;
	virtual QStringList getAbsoluteFilePathToAllTools() = 0;
	virtual bool verifyTool(QString uid) = 0;
};

} // namespace cx

#endif // CXTRACKERCONFIGURATION_H
