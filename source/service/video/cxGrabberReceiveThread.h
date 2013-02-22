
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
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"
#include "sscImage.h"
#include "sscTool.h"

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

typedef boost::shared_ptr<class GrabberReceiveThread> GrabberReceiveThreadPtr;

/** \brief Base class for receiving messages from grabber.
 *
 * Subclass to implement for a specific protocol.
 * Supported messages:
 *  - ssc::Image : contains vtkImageData, timestamp, uid, all else is discarded.
 *  - ssc::ProbeData : contains sector and image definition, temporal cal is discarded.
 *
 * \ingroup cxServiceVideo
 * \date Oct 11, 2012
 * \author christiana
 */
class GrabberReceiveThread: public QThread
{
Q_OBJECT
public:
	GrabberReceiveThread(QObject* parent = NULL);
	virtual ~GrabberReceiveThread() {}
	virtual ssc::ImagePtr getLastImageMessage(); // threadsafe
	virtual ssc::ProbeData getLastSonixStatusMessage(); // threadsafe
	virtual QString hostDescription() const = 0; // threadsafe

signals:
	void imageReceived();
	void sonixStatusReceived();
	void fps(double);
	void connected(bool on);
	void stopInternal();

protected:
	cx::CyclicActionTimer mFPSTimer;
	/** Add the message to a thread-safe queue.
	 * Tests if the time stamps of image messages should be calibrated based on the computer clock.
	 * Time stamps only need to be synched if set on another computer that is
	 * not synched, e.g. the Ultrasonix scanner
	 * \param[in] imgMsg Incoming image message
	 */
	void addImageToQueue(ssc::ImagePtr imgMsg);
	void addSonixStatusToQueue(ssc::ProbeData msg);
	void calibrateTimeStamp(ssc::ImagePtr imgMsg);

private:
	QMutex mImageMutex;
	QMutex mSonixStatusMutex;
	std::list<ssc::ImagePtr> mMutexedImageMessageQueue;
	std::list<ssc::ProbeData> mMutexedSonixStatusMessageQueue;

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
