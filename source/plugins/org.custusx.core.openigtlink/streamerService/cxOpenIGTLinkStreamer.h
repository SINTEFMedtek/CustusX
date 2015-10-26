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
#ifndef CXOPENIGTLINKSTREAMER_H
#define CXOPENIGTLINKSTREAMER_H

#include "org_custusx_core_openigtlink_Export.h"
#include "cxStreamer.h"

#include "cxImage.h"
#include "cxProbeDefinition.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"

namespace cx
{

/**
 * Streamer that listens to an OpenIGTLink connection, then
 * streams the incoming data.
 *
 * \addtogroup org_custusx_core_openigtlink
 * \author Janne Beate Bakeng, SINTEF
 * \date 2015-03-25
 */
class org_custusx_core_openigtlink_EXPORT OpenIGTLinkStreamer : public Streamer
{
    Q_OBJECT

public:
    OpenIGTLinkStreamer();
    virtual ~OpenIGTLinkStreamer();

    virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
    virtual bool isStreaming();
	virtual QString getType();

public slots:
    void receivedConnected();
    void receivedDisconnected();
    void receivedError();
    void receivedImage(ImagePtr image);
    void receivedProbedefinition(QString not_used, ProbeDefinitionPtr probedef);
//    void receiveIgtlImage(IGTLinkImageMessage::Pointer igtlimage);
//    void receivedUSStatusMessage(IGTLinkUSStatusMessage::Pointer message);

protected slots:
    virtual void streamSlot();

private:
    SenderPtr mSender;
    IGTLinkUSStatusMessage::Pointer mUnsentUSStatusMessage; ///< received message, will be added to queue when next image arrives

};
typedef boost::shared_ptr<OpenIGTLinkStreamer> OpenIGTLinkStreamerPtr;

} // namespace cx

#endif // CXOPENIGTLINKSTREAMER_H
