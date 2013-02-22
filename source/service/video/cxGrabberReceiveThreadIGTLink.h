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
#ifndef CXGRABBERRECEIVETHREADIGTLINK_H_
#define CXGRABBERRECEIVETHREADIGTLINK_H_

#include <vector>
#include <QtCore>
#include <QTcpSocket>
#include "boost/shared_ptr.hpp"
class QTcpSocket;
#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"
#include "cxGrabberReceiveThread.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */

typedef boost::shared_ptr<class GrabberReceiveThreadIGTLink> GrabberReceiveThreadIGTLinkPtr;

/**\brief Client thread for OpenIGTLink messaging.
 * \ingroup cxServiceVideo
 *
 *
 */
class GrabberReceiveThreadIGTLink: public GrabberReceiveThread
{
Q_OBJECT
public:
	GrabberReceiveThreadIGTLink(QString address, int port, QObject* parent = NULL);
	virtual QString hostDescription() const; // threadsafe
	~GrabberReceiveThreadIGTLink() {}

protected:
	virtual void run();

private slots:
	void readyReadSlot();

	void hostFoundSlot();
	void connectedSlot();
	void disconnectedSlot();
	void errorSlot(QAbstractSocket::SocketError);

private:
	bool ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
	bool ReceiveSonixStatus(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
	bool readOneMessage();
	void addToQueue(IGTLinkUSStatusMessage::Pointer msg);
	void addToQueue(IGTLinkImageMessage::Pointer msg);

	bool mHeadingReceived;
	QString mAddress;
	int mPort;
	QTcpSocket* mSocket;
	igtl::MessageHeader::Pointer mHeaderMsg;
	IGTLinkUSStatusMessage::Pointer mUnsentUSStatusMessage; ///< received message, will be added to queue when next image arrives

};

/**
 * @}
 */
} //end namespace cx

#endif /* CXGRABBERRECEIVETHREADIGTLINK_H_ */
