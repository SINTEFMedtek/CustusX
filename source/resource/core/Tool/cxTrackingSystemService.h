/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSYSTEMSERVICE_H
#define CXTRACKINGSYSTEMSERVICE_H

#include "cxResourceExport.h"
#include "cxTool.h"

#define TrackingSystemService_iid "cx::TrackingSystemService"

namespace cx
{

typedef boost::shared_ptr<class TrackingSystemService> TrackingSystemServicePtr;
typedef boost::shared_ptr<class TrackerConfiguration> TrackerConfigurationPtr;

/** \brief Tracking System Service
 *
 * Represents one tracking system consisting of a collection of tools.
 * Several such may exist.
 *
 * \ingroup cx_resource_core_tool
 *  \date 2014-09-30
 *  \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT TrackingSystemService : public QObject
{
	Q_OBJECT
public:
	TrackingSystemService();
	virtual ~TrackingSystemService() {}

	virtual QString getUid() const = 0;
	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val) = 0; ///< asynchronously request a state. Wait for signal stateChanged()
	virtual std::vector<ToolPtr> getTools() = 0;
	virtual TrackerConfigurationPtr getConfiguration() = 0;
	virtual ToolPtr getReference() = 0; ///< reference tool used by entire tracking service - NOTE: system fails if several TrackingSystemServices define this tool

	virtual void setConfigurationFile(QString configurationFile);
	virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

signals:
	void stateChanged();

protected slots:
	virtual void deconfigure() {} ///< deconfigures the software
	virtual void configure() {} ///< sets up the software

	virtual void initialize(); ///< connects to the hardware
	virtual void uninitialize(); ///< disconnects from the hardware
	virtual void startTracking(); ///< starts tracking
	virtual void stopTracking(); ///< stops tracking
protected:
	Tool::State mState;
	QString mConfigurationFilePath; ///< path to the configuration file
	QString mLoggingFolder; ///< path to where logging should be saved

	virtual bool isConfigured() const;
	virtual bool isInitialized() const;
	virtual bool isTracking() const;

	void internalSetState(Tool::State val);
};
} //namespace cx
Q_DECLARE_INTERFACE(cx::TrackingSystemService, TrackingSystemService_iid)

#endif // CXTRACKINGSYSTEMSERVICE_H
