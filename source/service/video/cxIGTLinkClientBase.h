
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


#ifndef CXIGTLinkClientBaseBASE_H_
#define CXIGTLinkClientBaseBASE_H_

#include <vector>
#include <QtCore>
#include <QTcpSocket>
#include "boost/shared_ptr.hpp"
class QTcpSocket;
#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include "cxRenderTimer.h"
#include "../../../modules/grabberCommon/cxIGTLinkUSStatusMessage.h"
#include "../grabberCommon/cxIGTLinkImageMessage.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */

typedef boost::shared_ptr<class IGTLinkClientBase> IGTLinkClientBasePtr;

/**\brief Base class Client thread for OpenIGTLink messaging.
 * \ingroup cxServiceVideo
 *
 *  \date Oct 11, 2012
 *  \author christiana
 *
 */
class IGTLinkClientBase: public QThread
{
Q_OBJECT
public:
	IGTLinkClientBase(QObject* parent = NULL) : QThread(parent) {  }
	virtual ~IGTLinkClientBase() {}
	virtual IGTLinkImageMessage::Pointer getLastImageMessage(); // threadsafe
	virtual IGTLinkUSStatusMessage::Pointer getLastSonixStatusMessage(); // threadsafe
	virtual QString hostDescription() const = 0; // threadsafe
	void stop(); ///< use instead of quit()

signals:
	void imageReceived();
	void sonixStatusReceived();
	void fps(double);
	void connected(bool on);
	void stopInternal();

protected slots:
	virtual void stopSlot() {}

protected:
	cx::RenderTimer mFPSTimer;
	void addImageToQueue(IGTLinkImageMessage::Pointer imgMsg);
	void addSonixStatusToQueue(IGTLinkUSStatusMessage::Pointer msg);

private:
	QMutex mImageMutex;
	QMutex mSonixStatusMutex;
	std::list<IGTLinkImageMessage::Pointer> mMutexedImageMessageQueue;
	std::list<IGTLinkUSStatusMessage::Pointer> mMutexedSonixStatusMessageQueue;
};

/**
 * @}
 */
} //end namespace cx

#endif /* CXIGTLinkClientBaseBASE_H_ */
