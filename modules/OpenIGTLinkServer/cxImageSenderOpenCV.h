/*
 * cxImageSenderOpenCV.h
 *
 *  \date Jun 21, 2011
 *      \author christiana
 */

#ifndef CXIMAGESENDEROPENCV_H_
#define CXIMAGESENDEROPENCV_H_

#include <QObject> //needed for the mocer when OpenCv is not used...

#ifdef USE_OpenCV
#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QSize>
class QTimer;
#include "igtlImageMessage.h"
#include <opencv2/highgui/highgui.hpp>
#include <QStringList>
#include "cxImageSenderFactory.h"
#include "../grabberCommon/cxIGTLinkImageMessage.h"

//class ImageSender : public QObject
//{
//	Q_OBJECT
//public:
//	ImageSender(QObject* parent = NULL) : QObject(parent) {}
//	virtual ~ImageSender() {}
//
//	virtual void initialize(StringMap arguments) = 0;
//	virtual void startStreaming(QTcpSocket* socket) = 0;
//	virtual void stopStreaming() = 0;
//
//	virtual QString getType() = 0;
//	virtual QStringList getArgumentDescription() = 0;
//};

namespace cx
{

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * This version uses openCV to grab images from a video file or camera
 */
class ImageSenderOpenCV: public ImageSender
{
Q_OBJECT

public:
	ImageSenderOpenCV(QObject* parent = NULL);
	virtual ~ImageSenderOpenCV() {}

	virtual void initialize(StringMap arguments);
	virtual void startStreaming(QTcpSocket* socket);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

protected:
private:
	QTcpSocket* mSocket;
	QTimer* mSendTimer;
	QTimer* mGrabTimer;
	StringMap mArguments;
	QSize mRescaleSize;
	void dumpProperties();
	IGTLinkImageMessage::Pointer getImageMessage();
	void dumpProperty(int val, QString name);

	void initialize_local();
	void deinitialize_local();

	cv::VideoCapture mVideoCapture; // OpenCV video grabber
	QDateTime mLastGrabTime;

private slots:
	//  void tick();
	void grab();
	void send();

};

}

#endif // USE_OpenCV
#endif /* CXIMAGESENDEROPENCV_H_ */
