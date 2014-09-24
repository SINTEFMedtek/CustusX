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
#ifndef CXIGTLinkedImageReceiverThread_H_
#define CXIGTLinkedImageReceiverThread_H_

#include "cxVideoServiceExport.h"

#include <vector>
#include <QTcpSocket>
#include "boost/shared_ptr.hpp"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"
#include "cxImageReceiverThread.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_video
 * @{
 */

typedef boost::shared_ptr<class IGTLinkedImageReceiverThread> IGTLinkedImageReceiverThreadPtr;

/**\brief Client thread for OpenIGTLink messaging.
 * \ingroup cx_service_video
 *
 *
 */
class cxVideoService_EXPORT IGTLinkedImageReceiverThread: public ImageReceiverThread
{
Q_OBJECT
public:
	IGTLinkedImageReceiverThread(QString address, int port, QObject* parent = NULL);
	virtual QString hostDescription() const; // threadsafe
	~IGTLinkedImageReceiverThread() {}

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

#endif /* CXIGTLinkedImageReceiverThread_H_ */
