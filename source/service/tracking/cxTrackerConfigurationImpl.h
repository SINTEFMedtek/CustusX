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
#ifndef CXTRACKERCONFIGURATIONIMPL_H
#define CXTRACKERCONFIGURATIONIMPL_H

#include "cxTrackerConfiguration.h"
#include "cxIgstkTool.h"
#include <QDir>

namespace cx
{

class TrackerConfigurationImpl : public TrackerConfiguration
{
public:
	virtual ~TrackerConfigurationImpl() {}

	virtual void saveConfiguration(const Configuration& config);
	virtual Configuration getConfiguration(QString uid);

	virtual QStringList getToolsGivenFilter(QStringList applicationsFilter,
											QStringList trackingsystemsFilter);
	virtual QString getToolName(QString uid);
	virtual QString getToolTrackingSystem(QString uid);
	virtual QString getToolPictureFilename(QString uid);
	virtual Tool getTool(QString uid);
	virtual QStringList getAbsoluteFilePathToAllTools();

private:
	QStringList filter(QStringList toolsToFilter, QStringList applicationsFilter,
			QStringList trackingsystemsFilter);
	IgstkTool::InternalStructure getToolInternal(QString toolAbsoluteFilePath);

};


} // namespace cx



#endif // CXTRACKERCONFIGURATIONIMPL_H
