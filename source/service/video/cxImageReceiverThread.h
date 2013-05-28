
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


#ifndef CXImageReceiverThread_H_
#define CXImageReceiverThread_H_

#include <vector>
#include "boost/shared_ptr.hpp"

#include <QThread>
#include <QMutex>

#include "sscImage.h"
#include "sscTool.h"

namespace cx
{
typedef boost::shared_ptr<class CyclicActionTimer> CyclicActionTimerPtr;

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

typedef boost::shared_ptr<class ImageReceiverThread> ImageReceiverThreadPtr;

/** \brief Base class for receiving images from a video stream.
 *
 * Subclass to implement for a specific protocol.
 * Supported messages:
 *  - ssc::Image : contains vtkImageData, timestamp, uid, all else is discarded.
 *  - ssc::ProbeData : contains sector and image definition, temporal cal is discarded.
 *
 * \ingroup cxServiceVideo
 * \date Oct 11, 2012
 * \author Christian Askeland, SINTEF
 */
class ImageReceiverThread: public QThread
{
Q_OBJECT
public:
	ImageReceiverThread(QObject* parent = NULL);
	virtual ~ImageReceiverThread() {}
	virtual ssc::ImagePtr getLastImageMessage(); // threadsafe, Threadsafe retrieval of last image message.
	virtual ssc::ProbeDataPtr getLastSonixStatusMessage(); // threadsafe,Threadsafe retrieval of last status message.
	virtual QString hostDescription() const = 0; // threadsafe

signals:
	void imageReceived();
	void sonixStatusReceived();
	void fps(double);
	void connected(bool on);
	void stopInternal();

protected:
	cx::CyclicActionTimerPtr mFPSTimer;
	/** Add the message to a thread-safe queue.
	 * Tests if the time stamps of image messages should be calibrated based on the computer clock.
	 * Time stamps only need to be synched if set on another computer that is
	 * not synched, e.g. the Ultrasonix scanner
	 * \param[in] imgMsg Incoming image message
	 */
	void addImageToQueue(ssc::ImagePtr imgMsg);
	void addSonixStatusToQueue(ssc::ProbeDataPtr msg); ///< add the message to a thread-safe queue
	void calibrateTimeStamp(ssc::ImagePtr imgMsg); ///< Calibrate the time stamps of the incoming message based on the computer clock. Calibration is based on an average of several of the last messages. The calibration is updated every 20-30 sec.

private:
	void reportFPS();
	bool imageComesFromActiveVideoSource(ssc::ImagePtr imgMsg);
	bool imageComesFromSonix(ssc::ImagePtr imgMsg);

	QMutex mImageMutex;
	QMutex mSonixStatusMutex;
	std::list<ssc::ImagePtr> mMutexedImageMessageQueue;
	std::list<ssc::ProbeDataPtr> mMutexedSonixStatusMessageQueue;

	double mLastReferenceTimestampDiff;
	bool mGeneratingTimeCalibration;
	QDateTime mLastSyncTime;
	std::vector<double> mLastTimeStamps;
};

/**
 * @}
 */
} //end namespace cx

#endif /* CXImageReceiverThread_H_ */
