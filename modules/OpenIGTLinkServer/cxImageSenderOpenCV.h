/*
 * cxImageSenderOpenCV.h
 *
 *  \date Jun 21, 2011
 *      \author christiana
 */

#ifndef CXIMAGESENDEROPENCV_H_
#define CXIMAGESENDEROPENCV_H_

#ifdef USE_OpenCV

#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QDateTime>
class QTimer;
#include "igtlImageMessage.h"
#include <opencv2/highgui/highgui.hpp>
#include <QStringList>
#include "cxImageSenderFactory.h"
#include "../grabberCommon/cxIGTLinkImageMessage.h"

namespace cx
{

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * This version uses openCV to grab images from a video file or camera
 */
class ImageSenderOpenCV: public QObject
{
Q_OBJECT

public:
	ImageSenderOpenCV(QTcpSocket* socket, StringMap arguments, QObject* parent = NULL);
	virtual ~ImageSenderOpenCV() {}

	static QString getType();
	static QStringList getArgumentDescription();

protected:
private:
	QTcpSocket* mSocket;
	QTimer* mSendTimer;
	QTimer* mGrabTimer;
	StringMap mArguments;
	void dumpProperties();
	IGTLinkImageMessage::Pointer getImageMessage();
	void dumpProperty(int val, QString name);

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
