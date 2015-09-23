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

#ifndef CXOPENIGTLINKTRACKINGSYSTEMSERVICE_H_
#define CXOPENIGTLINKTRACKINGSYSTEMSERVICE_H_

#include <QThread>

#include "cxTrackingSystemService.h"
#include "org_custusx_core_openigtlink_Export.h"

namespace cx
{
class OpenIGTLinkClient;
typedef boost::shared_ptr<class OpenIGTLinkTool> OpenIGTLinkToolPtr;
typedef boost::shared_ptr<class OpenIGTLinkClientThreadHandler> OpenIGTLinkClientThreadHandlerPtr;

/**
 * Tracking system service that gets tracking information from an OpenIGTLink source
 *
 * \ingroup org_custusx_core_openigtlink
 *
 * \date 2015-03-03
 * \author Janne Beate Bakeng
 */
class org_custusx_core_openigtlink_EXPORT OpenIGTLinkTrackingSystemService : public TrackingSystemService
{
    Q_OBJECT
    Q_INTERFACES(cx::TrackingSystemService)

public:
	OpenIGTLinkTrackingSystemService(OpenIGTLinkClientThreadHandlerPtr connection);
    virtual ~OpenIGTLinkTrackingSystemService();

    virtual QString getUid() const;
    virtual Tool::State getState() const;
    virtual void setState(const Tool::State val); ///< asynchronously request a state. Wait for signal stateChanged()
    virtual std::vector<ToolPtr> getTools();
    virtual TrackerConfigurationPtr getConfiguration();
    virtual ToolPtr getReference(); ///< reference tool used by entire tracking service - NOTE: system fails if several TrackingSystemServices define this tool

    virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

signals:
    void connectToServer();
    void disconnectFromServer();

private slots:
    void configure(); ///< sets up the software
    virtual void deconfigure(); ///< deconfigures the software
    void initialize(); ///< connects to the hardware
    void uninitialize(); ///< disconnects from the hardware
    void startTracking(); ///< starts tracking
    void stopTracking(); ///< stops tracking

    void serverIsConfigured();
    void serverIsDeconfigured();
    void serverIsConnected();
    void serverIsDisconnected();

    void receiveTransform(QString devicename, Transform3D transform, double timestamp);
    void receiveCalibration(QString devicename, Transform3D calibration);
    void receiveProbedefinition(QString devicename, ProbeDefinitionPtr definition);

private:
    void internalSetState(Tool::State state);
    OpenIGTLinkToolPtr getTool(QString devicename);

    Tool::State mState;
    std::map<QString, OpenIGTLinkToolPtr> mTools;
    ToolPtr mReference;
	OpenIGTLinkClientThreadHandlerPtr mConnection;

};
typedef boost::shared_ptr<OpenIGTLinkTrackingSystemService> OpenIGTLinkTrackingSystemServicePtr;

} /* namespace cx */

#endif /* CXOPENIGTLINKTRACKINGSYSTEMSERVICE_H_ */

