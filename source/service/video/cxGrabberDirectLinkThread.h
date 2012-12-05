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

#ifndef CXGRABBERDIRECTLINKTHREAD_H_
#define CXGRABBERDIRECTLINKTHREAD_H_

#include <vector>
#include <QtCore>
#include <QTcpSocket>
#include "boost/shared_ptr.hpp"
class QTcpSocket;
#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include "../../../modules/grabberCommon/cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkClientBase.h"
#include "cxImageSender.h"
#include "cxGrabberSender.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */

typedef boost::shared_ptr<class GrabberDirectLinkThread> GrabberDirectLinkThreadPtr;

/**\brief Client thread for OpenIGTLink messaging.
 * \ingroup cxServiceVideo
 *
 *  \date Oct 11, 2012
 *  \author christiana
 */
class GrabberDirectLinkThread: public IGTLinkClientBase
{
Q_OBJECT
public:
	GrabberDirectLinkThread(std::map<QString, QString> args, QObject* parent = NULL);
	virtual QString hostDescription() const; // threadsafe

protected:
	virtual void run();

private slots:
	void newImageSlot();
	void newUSStatusSlot();
//	void stopSlot();

private:
	std::map<QString, QString> mArguments;
	ImageSenderPtr mImageSender;
	GrabberSenderDirectLinkPtr mGrabberBridge;
};

/**
 * @}
 */
} //end namespace cx

#endif /* CXGRABBERDIRECTLINKTHREAD_H_ */
