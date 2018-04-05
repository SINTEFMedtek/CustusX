/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXOPENIGTLINKTRACKINGSYSTEMSERVICE_H_
#define CXOPENIGTLINKTRACKINGSYSTEMSERVICE_H_

#include "org_custusx_core_openigtlink3_Export.h"

#include <QThread>

#include "cxNetworkHandler.h"
#include "cxTrackingSystemService.h"

namespace cx
{
typedef boost::shared_ptr<class NetworkHandler> NetworkHandlerPtr;
typedef boost::shared_ptr<class OpenIGTLinkTool> OpenIGTLinkToolPtr;

/**
 * Tracking system service that gets tracking information from an OpenIGTLink source
 *
 * \ingroup org_custusx_core_openigtlink
 *
 * \date 2015-03-03
 * \author Janne Beate Bakeng
 */
class org_custusx_core_openigtlink3_EXPORT OpenIGTLinkTrackingSystemService : public TrackingSystemService
{
    Q_OBJECT
    Q_INTERFACES(cx::TrackingSystemService)

public:
	OpenIGTLinkTrackingSystemService(NetworkHandlerPtr networkHandler);
    virtual ~OpenIGTLinkTrackingSystemService();

    virtual QString getUid() const;
    virtual void setState(const Tool::State val); ///< asynchronously request a state. Wait for signal stateChanged()
    virtual std::vector<ToolPtr> getTools();
    virtual TrackerConfigurationPtr getConfiguration();
    virtual ToolPtr getReference(); ///< reference tool used by entire tracking service - NOTE: system fails if several TrackingSystemServices define this tool

protected:
	void internalSetState(Tool::State val);

protected slots:
	virtual void configure(); ///< sets up the software
	virtual void deconfigure(); ///< deconfigures the software

private slots:
	void serverIsConnected();
	void serverIsDisconnected();

	void receiveTransform(QString devicename, Transform3D transform, double timestamp);
	void receiveCalibration(QString devicename, Transform3D calibration);
	void receiveProbedefinition(QString devicename, ProbeDefinitionPtr definition);

private:
	OpenIGTLinkToolPtr getTool(QString devicename);

	std::map<QString, OpenIGTLinkToolPtr> mTools;
	ToolPtr mReference;
	NetworkHandlerPtr mNetworkHandler;

signals:
	void setInternalState(const Tool::State val);
};
typedef boost::shared_ptr<OpenIGTLinkTrackingSystemService> OpenIGTLinkTrackingSystemServicePtr;

} /* namespace cx */

#endif /* CXOPENIGTLINKTRACKINGSYSTEMSERVICE_H_ */

