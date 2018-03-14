/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXGrabberSenderQTcpSocket_H_
#define CXGrabberSenderQTcpSocket_H_

#include "cxGrabberExport.h"

#include "cxSenderImpl.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <qtcpsocket.h>
#include "igtlImageMessage.h"
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxImage.h"
#include "cxTool.h"

namespace cx
{

/**
* \file
* \addtogroup cx_resource_videoserver
* @{
*/

class cxGrabber_EXPORT GrabberSenderQTcpSocket : public SenderImpl
{
public:
	explicit GrabberSenderQTcpSocket(QTcpSocket* socket);
	virtual ~GrabberSenderQTcpSocket() {}

	bool isReady() const;

protected:
	virtual void send(igtl::ImageMessage::Pointer msg);
	virtual void send(IGTLinkUSStatusMessage::Pointer msg);
	virtual void send(ImagePtr msg);
	virtual void send(ProbeDefinitionPtr msg);

private:
	QTcpSocket* mSocket;
	int mMaxBufferSize;
};

/**
* @}
*/

} /* namespace cx */
#endif /* CXGrabberSenderQTcpSocket__H_ */
