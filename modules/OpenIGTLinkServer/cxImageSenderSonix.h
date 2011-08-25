/*
 * cxImageSenderSonix.h
 *
 *  Created on: Aug 15, 2011
 *      Author: Ole Vegard Solberg
 */

#ifndef CXIMAGESENDERSONIX_H_
#define CXIMAGESENDERSONIX_H_

#ifdef WIN32


#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
class QTimer;
#include <QStringList>
#include <QMetaType>
#include "cxImageSenderFactory.h"

#include "../grabberCommon/cxIGTLinkImageMessage.h"
#include "../grabberCommon/cxIGTLinkSonixStatusMessage.h"
#include "vtkSonixVideoSource.h"
#include "SonixHelper.h"

namespace cx
{

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * This version uses openCV to grab images from the Ultrasonix scanner
 */
class ImageSenderSonix : public QObject
{
  Q_OBJECT

public:
  ImageSenderSonix(QTcpSocket* socket, StringMap arguments, QObject* parent = NULL);
  ~ImageSenderSonix();

  static QString getType();
  static QStringList getArgumentDescription();

protected:
private:
  QTcpSocket* mSocket;
  QTimer* mTimer;
  StringMap mArguments;

  vtkSonixVideoSource*  mSonixGrabber; ///< Ultrasonix video grabber
  SonixHelper*          mSonixHelper; ///< Support Qt functionality to vtkSonixVideoSource

//  void dumpProperties();
//  igtl::ImageMessage::Pointer getImageMessage();
//  void dumpProperty(int val, QString name);

  //cv::VideoCapture mVideoCapture; // OpenCV video grabber


  IGTLinkImageMessage::Pointer convertFrame(Frame& frame);
  IGTLinkSonixStatusMessage::Pointer getFrameStatus(Frame& frame);

private slots:
  //void tick();
  void receiveFrameSlot(Frame& frame);

};

}//namespace cx

//Declaration of a frame metatype needed to be able to send frame as a signal.
typedef cx::Frame Frame;
Q_DECLARE_METATYPE(Frame)

#endif // WIN32

#endif /* CXIMAGESENDERSONIX_H_ */
