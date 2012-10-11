/*
 * cxGrabberSender.h
 *
 *  Created on: Oct 11, 2012
 *      Author: christiana
 */

#ifndef CXGRABBERSENDER_H_
#define CXGRABBERSENDER_H_

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <qtcpsocket.h>
#include "../grabberCommon/cxIGTLinkImageMessage.h"
#include "../grabberCommon/cxIGTLinkUSStatusMessage.h"

namespace cx
{

/**Interface for sending grabbed data from the ImageSender classes further to either
 * a TCP/IP socket or directly via a Qt signal.
 *
 */
class GrabberSender : public QObject
{
	Q_OBJECT
public:
	GrabberSender() {}
	virtual ~GrabberSender() {}

	/**Return if sender is ready to send another message. If !isReady(),
	 * calls to send() will fail.
	 */
	virtual bool isReady() const = 0;
	/**Send an image message, NOT packed.
	 */
	virtual void send(IGTLinkImageMessage::Pointer msg) = 0;
	/**Send an US status message, NOT packed
	 */
	virtual void send(IGTLinkUSStatusMessage::Pointer msg) = 0;
};
typedef boost::shared_ptr<GrabberSender> GrabberSenderPtr;

/**
 *
 */
class GrabberSenderQTcpSocket : public GrabberSender
{
public:
	explicit GrabberSenderQTcpSocket(QTcpSocket* socket);
	virtual ~GrabberSenderQTcpSocket() {}

	bool isReady() const;
	virtual void send(IGTLinkImageMessage::Pointer msg);
	virtual void send(IGTLinkUSStatusMessage::Pointer msg);
private:
	QTcpSocket* mSocket;
	int mMaxBufferSize;
};


/**
 *
 */
class GrabberSenderDirectLink : public GrabberSender
{
	Q_OBJECT
public:
	GrabberSenderDirectLink() {}
	virtual ~GrabberSenderDirectLink() {}

	bool isReady() const;
	virtual void send(IGTLinkImageMessage::Pointer msg);
	virtual void send(IGTLinkUSStatusMessage::Pointer msg);

	IGTLinkImageMessage::Pointer popImage();
	IGTLinkUSStatusMessage::Pointer popUSStatus();

signals:
	void newImage();
	void newUSStatus();
private:
	IGTLinkImageMessage::Pointer mImage;
	IGTLinkUSStatusMessage::Pointer mUSStatus;

};
typedef boost::shared_ptr<GrabberSenderDirectLink> GrabberSenderDirectLinkPtr;

} /* namespace cx */
#endif /* CXGRABBERSENDER_H_ */
