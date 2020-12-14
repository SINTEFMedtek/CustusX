/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXOPENIGTLINKSTREAMER_H
#define CXOPENIGTLINKSTREAMER_H

#include "org_custusx_core_openigtlink3_Export.h"
#include "cxStreamer.h"

#include "cxImage.h"
#include "cxProbeDefinition.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"

namespace cx
{

/**
 * Streamer that listens to an OpenIGTLink3 connection, then
 * streams the incoming data.
 *
 * \addtogroup org_custusx_core_openigtlink3
 * \author Janne Beate Bakeng, SINTEF
 * \date 2016-06-24
 */
class org_custusx_core_openigtlink3_EXPORT OpenIGTLinkStreamer : public Streamer
{
    Q_OBJECT

public:
    OpenIGTLinkStreamer();
    virtual ~OpenIGTLinkStreamer();

    virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
    virtual bool isStreaming();
	virtual QString getType();

signals:
		void stoppedStreaming();

public slots:
    void receivedConnected();
    void receivedDisconnected();
    void receivedError();
    void receivedImage(ImagePtr image);
    void receiveProbedefinition(QString not_used, ProbeDefinitionPtr probedef);

protected slots:
    virtual void streamSlot();

private:
    SenderPtr mSender;

};
typedef boost::shared_ptr<OpenIGTLinkStreamer> OpenIGTLinkStreamerPtr;

} // namespace cx

#endif // CXOPENIGTLINKSTREAMER_H
