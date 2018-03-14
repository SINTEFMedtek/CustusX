/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIGTLINKCLIENTSTREAMER_H
#define CXIGTLINKCLIENTSTREAMER_H

#include "cxStreamer.h"
#include "org_custusx_core_video_Export.h"
#include <QAbstractSocket>
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxStreamedTimestampSynchronizer.h"

class QTcpSocket;

namespace cx
{

/**
 * Streamer that listens to an IGTLink connection, then
 * streams the incoming data.
 *
 * \addtogroup org_custusx_core_video
 * \author Christian Askeland, SINTEF
 * \date 2014-11-20
 */
class org_custusx_core_video_EXPORT IGTLinkClientStreamer: public Streamer
{
Q_OBJECT

public:
	IGTLinkClientStreamer();
	virtual ~IGTLinkClientStreamer();

	void setAddress(QString address, int port);

	virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
	virtual bool isStreaming();


private slots:
	virtual void streamSlot() {}
private slots:
	void readyReadSlot();

	void hostFoundSlot();
	void connectedSlot();
	void disconnectedSlot();
	void errorSlot(QAbstractSocket::SocketError);

private:
	SenderPtr mSender;

	virtual QString hostDescription() const; // threadsafe
	bool ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
	bool ReceiveSonixStatus(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
	bool readOneMessage();
	void addToQueue(IGTLinkUSStatusMessage::Pointer msg);
	void addToQueue(igtl::ImageMessage::Pointer msg);
	bool multipleTryConnectToHost();
	bool tryConnectToHost();

	bool mHeadingReceived;
	QString mAddress;
	int mPort;
    StreamedTimestampSynchronizer mStreamSynchronizer;
    boost::shared_ptr<QTcpSocket> mSocket;
	igtl::MessageHeader::Pointer mHeaderMsg;
	IGTLinkUSStatusMessage::Pointer mUnsentUSStatusMessage; ///< received message, will be added to queue when next image arrives


};
typedef boost::shared_ptr<class IGTLinkClientStreamer> IGTLinkClientStreamerPtr;

} // namespace cx

#endif // CXIGTLINKCLIENTSTREAMER_H
