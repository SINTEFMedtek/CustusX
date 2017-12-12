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
