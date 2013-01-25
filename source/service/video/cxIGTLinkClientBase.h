
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


class AbsDoubleLess
{
public:
	AbsDoubleLess(double center) : mCenter(center) { };

  bool operator()(const double& d1, const double& d2)
  {
    return fabs(d1 - mCenter) < fabs(d2 - mCenter);
  }

  double mCenter;
};

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
	IGTLinkClientBase(QObject* parent = NULL);
	virtual ~IGTLinkClientBase() {}
	virtual IGTLinkImageMessage::Pointer getLastImageMessage(); // threadsafe
	virtual IGTLinkUSStatusMessage::Pointer getLastSonixStatusMessage(); // threadsafe
	virtual QString hostDescription() const = 0; // threadsafe
//	void stop(); ///< use instead of quit()

signals:
	void imageReceived();
	void sonixStatusReceived();
	void fps(double);
	void connected(bool on);
	void stopInternal();

//protected slots:
//	virtual void stopSlot() {}

protected:
	cx::CyclicActionTimer mFPSTimer;
	/** Add the message to a thread-safe queue.
	 * Tests if the time stamps of image messages should be calibrated based on the computer clock.
	 * Time stamps only need to be synched if set on another computer that is
	 * not synched, e.g. the Ultrasonix scanner
	 * \param[in] imgMsg Incoming image message
	 */
	void addImageToQueue(IGTLinkImageMessage::Pointer imgMsg);
	void addSonixStatusToQueue(IGTLinkUSStatusMessage::Pointer msg);
	void calibrateTimeStamp(IGTLinkImageMessage::Pointer imgMsg);

private:
	QMutex mImageMutex;
	QMutex mSonixStatusMutex;
	std::list<IGTLinkImageMessage::Pointer> mMutexedImageMessageQueue;
	std::list<IGTLinkUSStatusMessage::Pointer> mMutexedSonixStatusMessageQueue;

	double mLastReferenceTimestampDiff;
	bool mGeneratingTimeCalibration;
	QDateTime mLastSyncTime;
	std::vector<double> mLastTimeStamps;
};

/**
 * @}
 */
} //end namespace cx

#endif /* CXIGTLinkClientBaseBASE_H_ */
