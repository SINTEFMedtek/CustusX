#ifndef CXIMAGESTREAMEROPENCV_H_
#define CXIMAGESTREAMEROPENCV_H_

#include <QObject> //needed for the mocer when OpenCv is not used...
#include "cxConfig.h"

#ifdef CX_USE_OpenCV
#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QSize>
class QTimer;
#include "igtlImageMessage.h"
//#include <opencv2/highgui/highgui.hpp>
#include <QStringList>
#include "cxImageSenderFactory.h"
#include "cxIGTLinkImageMessage.h"
#include "cxGrabberSender.h"

namespace cv
{
	class VideoCapture;
}

namespace cx
{
typedef boost::shared_ptr<cv::VideoCapture> VideoCapturePtr;

/**
 * An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * \author Christian Askeland, SINTEF
 * \date Jun 21, 2011
 *
 * This version uses openCV to grab images from a video file or camera
 */
class ImageStreamerOpenCV: public ImageStreamer
{
Q_OBJECT

public:
	ImageStreamerOpenCV();
	virtual ~ImageStreamerOpenCV();

	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

private slots:
	void grab();
	void send();

private:
	void dumpProperties();
	IGTLinkImageMessage::Pointer getImageMessage();
	void dumpProperty(int val, QString name);

	void initialize_local();
	void deinitialize_local();

//	GrabberSenderPtr mSender;
//	QTimer* mSendTimer;
	QTimer* mGrabTimer;
//	StringMap mArguments;
	QSize mRescaleSize;

	VideoCapturePtr mVideoCapture; // OpenCV video grabber
	QDateTime mLastGrabTime;
	QTime mCounter;
	bool mAvailableImage;
};

}

#endif // CX_USE_OpenCV
#endif /* CXIMAGESTREAMEROPENCV_H_ */
