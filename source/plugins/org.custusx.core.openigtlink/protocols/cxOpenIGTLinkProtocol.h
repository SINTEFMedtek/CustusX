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


#ifndef CXOPENIGTLINKPROTOCOL_H
#define CXOPENIGTLINKPROTOCOL_H

#include "cxProtocol.h"

#include <QMutex>
#include "igtlMessageHeader.h"
#include "igtlPolyDataMessage.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlStatusMessage.h"
#include "igtlStringMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"

#define CX_OPENIGTLINK_CHANNEL_NAME "OpenIGTLink"

namespace cx
{

class OpenIGTLinkProtocol : public Protocol
{
    Q_OBJECT
public:
    OpenIGTLinkProtocol();
    virtual ~OpenIGTLinkProtocol(){};

    virtual bool readyToReceiveData();

    //TODO refactor to use pack system
	virtual EncodedPackagePtr encode(ImagePtr image);
	virtual EncodedPackagePtr encode(MeshPtr data);

    virtual void translate(const igtl::MessageHeader::Pointer &header, const igtl::MessageBase::Pointer &body);
    virtual void translate(const igtl::TransformMessage::Pointer body);
    virtual void translate(const igtl::ImageMessage::Pointer body);
    virtual void translate(const igtl::PolyDataMessage::Pointer body);
    virtual void translate(const igtl::StatusMessage::Pointer body);
    virtual void translate(const igtl::StringMessage::Pointer body);
    virtual void translate(const IGTLinkUSStatusMessage::Pointer body);


protected:
    void writeAcceptingMessage(igtl::MessageBase* body) const;
    void writeNotSupportedMessage(igtl::MessageBase *body) const;
	virtual PATIENT_COORDINATE_SYSTEM coordinateSystem() const { return pcsLPS; }
	virtual bool doCRC() const;

protected slots:
    void processPack();

private:
    igtl::MessageHeader::Pointer mHeader;
    igtl::MessageBase::Pointer mBody;
    bool mReadyToReceive;

    void setReadyToReceive(bool ready);

    void prepareHeader(const igtl::MessageHeader::Pointer &header) const;
    void prepareBody(const igtl::MessageBase::Pointer &header, igtl::MessageBase::Pointer &body);
    template <typename T>
    void prepareBody(const igtl::MessageHeader::Pointer &header, igtl::MessageBase::Pointer &body);
    void prepareHeaderPack(igtl::MessageHeader::Pointer &message);
    void prepareBodyPack(igtl::MessageBase::Pointer &message);
    bool unpackHeader(const igtl::MessageBase::Pointer &header) const;
    bool unpackBody(const igtl::MessageBase::Pointer &body);
    void getReadyToReceiveBody();
    void getReadyToReceiveHeader();
    bool isValid(const igtl::MessageBase::Pointer &msg) const;
    bool isSupportedBodyType(QString type) const;

};

} //namespace cx

#endif // CXOPENIGTLINKPROTOCOL_H
