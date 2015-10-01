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

    virtual void translate(const igtl::MessageHeader::Pointer &header, const igtl::MessageBase::Pointer &body);
    virtual void translate(const igtl::TransformMessage::Pointer body);
    virtual void translate(const igtl::ImageMessage::Pointer body);
    virtual void translate(const igtl::PolyDataMessage::Pointer body);
    virtual void translate(const igtl::StatusMessage::Pointer body);
    virtual void translate(const igtl::StringMessage::Pointer body);
    virtual void translate(const IGTLinkUSStatusMessage::Pointer body);
    virtual void translate(const IGTLinkImageMessage::Pointer body);

signals:
    void igtlimage(IGTLinkImageMessage::Pointer igtlimage);
    void usstatusmessage(IGTLinkUSStatusMessage::Pointer msg);
    void probedefinition(QString devicename, ProbeDefinitionPtr definition);

protected:
    void writeAcceptingMessage(igtl::MessageBase* body) const;
    void writeNotSupportedMessage(igtl::MessageBase *body) const;

protected slots:
    void processPack();

private:
    igtl::MessageHeader::Pointer mHeader;
    igtl::MessageBase::Pointer mBody;
    QMutex mReadyReadMutex;
    bool mReadyToReceive;

    void setReadyToReceive(bool ready);

    void prepareHeader(const igtl::MessageHeader::Pointer &header) const;
    void prepareBody(const igtl::MessageBase::Pointer &header, igtl::MessageBase::Pointer &body);
    template <typename T>
    void prepareBody(const igtl::MessageHeader::Pointer &header, igtl::MessageBase::Pointer &body);
    void preparePack(void *pointer, int size);
    bool unpackHeader(const igtl::MessageBase::Pointer &header) const;
    bool unpackBody(const igtl::MessageBase::Pointer &body);
    void getReadyToReceiveBody();
    void getReadyToReceiveHeader();
    bool isValid(const igtl::MessageBase::Pointer &msg) const;
    bool isSupportedBodyType(QString type) const;

};

} //namespace cx

#endif // CXOPENIGTLINKPROTOCOL_H
