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


#ifndef CXNETWORKCONNECTION_H_
#define CXNETWORKCONNECTION_H_

#include "org_custusx_core_openigtlink_Export.h"

#include <map>
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlStatusMessage.h"
#include "igtlStringMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

#include "cxSocketConnection.h"
#include "cxTransform3D.h"
#include "cxImage.h"
#include "cxProbeDefinition.h"
#include "cxLogger.h"
#include "cxProtocol.h"
#include "boost/function.hpp"

typedef boost::shared_ptr<QThread> QThreadPtr;

namespace cx {

typedef boost::shared_ptr<class NetworkConnection> NetworkConnectionPtr;

/**
 * @brief The NetworkConnection class handles incoming OpenIGTLink packages.
 *
 * To specify how packages should be handled you can specify different kind of
 * supported dialects, which are a way to handle the way different OpenIGTLink
 * servers send packages.
 *
 */

class org_custusx_core_openigtlink_EXPORT NetworkConnection : public SocketConnection
{
    Q_OBJECT
public:

	explicit NetworkConnection(QString uid, QObject *parent = 0);
	virtual ~NetworkConnection();

    //thread safe
	QString getUid() const { return mUid; }
	virtual void setConnectionInfo(ConnectionInfo info);
	QStringList getAvailableDialects() const;
	void invoke(boost::function<void()> func);

	// not thread-safe: use invoke to call in this thread
	void sendImage(ImagePtr image);
	void sendMesh(MeshPtr image);

signals:
    void transform(QString devicename, Transform3D transform, double timestamp);
    void calibration(QString devicename, Transform3D calibration);
    void image(ImagePtr image);
	void mesh(MeshPtr image);
	void probedefinition(QString devicename, ProbeDefinitionPtr definition);
    void igtlimage(IGTLinkImageMessage::Pointer igtlimage);
    void usstatusmessage(IGTLinkUSStatusMessage::Pointer message);

private slots:
    virtual void internalDataAvailable();
	void onInvoke(boost::function<void()> func);

protected:

private:
    ProtocolPtr initDialect(ProtocolPtr value);
	void setDialect(QString dialectname);
	bool receiveHeader(const igtl::MessageHeader::Pointer header) const;
    bool receiveBody(const igtl::MessageHeader::Pointer header);
	qint64 skip(qint64 maxSizeBytes) const;

    template <typename T>
    bool receive(const igtl::MessageBase::Pointer header);

    QMutex mMutex;

    igtl::MessageHeader::Pointer mHeader;
    bool mHeaderReceived;

    ProtocolPtr mDialect;
    typedef std::map<QString, ProtocolPtr> DialectMap;
    DialectMap mAvailableDialects;
	const QString mUid;
};



} //namespace cx

#endif // CXNETWORKCONNECTION_H_
