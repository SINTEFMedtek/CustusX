/*
 * cxImageSenderOpenCV.h
 *
 *  Created on: Jun 21, 2011
 *      Author: christiana
 */

#ifndef CXIMAGESENDEROPENCV_H_
#define CXIMAGESENDEROPENCV_H_

#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
class QTimer;
#include "igtlImageMessage.h"
#include <opencv2/highgui/highgui.hpp>

namespace cx
{

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * This version uses openCV to grab images from a video file or camera
 */
class ImageSenderOpenCV : public QObject
{
  Q_OBJECT

public:
  ImageSenderOpenCV(QTcpSocket* socket, QString imageFileDir, QObject* parent = NULL);
  virtual ~ImageSenderOpenCV() {}
protected:
private:
  QTcpSocket* mSocket;
  QTimer* mTimer;
  QString mImageFileDir;
  void dumpProperties();
  igtl::ImageMessage::Pointer getImageMessage();
  void dumpProperty(int val, QString name);

  cv::VideoCapture mVideoCapture; // OpenCV video grabber

private slots:
  void tick();

};

}

#endif /* CXIMAGESENDEROPENCV_H_ */
