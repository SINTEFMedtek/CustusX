#ifndef CXIMAGESTREAMERSONIX_H_
#define CXIMAGESTREAMERSONIX_H_

#ifdef CX_WIN32

#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QStringList>
#include <QMetaType>
#include <QMutex>
#include <vtkSmartPointer.h>
#include "cxImageSenderFactory.h"

#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "vtkSonixVideoSource.h"
#include "SonixHelper.h"
#include "cxImageStreamer.h"

class QTimer;
typedef vtkSmartPointer<class vtkSonixVideoSource> vtkSonixVideoSourcePtr;

namespace cx
{

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * \date Aug 15, 2011
 * \author Ole Vegard Solberg, SINTEF
 *
 * This version uses openCV to grab images from the Ultrasonix scanner
 */
class ImageStreamerSonix : public CommandLineStreamer
{
  Q_OBJECT

public:
  ImageStreamerSonix();
  ~ImageStreamerSonix();
  
	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

public slots:
  void sendOpenIGTLinkImageSlot(int sendNumberOfMessages); ///< Gets the oldest frame from the internal queue and sends it to the socket.
  void sendOpenIGTLinkStatusSlot(int sendNumberOfMessages); ///< Gets the oldest status message from the internal queue and sends it to the socket.

signals:
    void imageOnQueue(int); ///< Emitted when there is a new igtl::ImageMessage is in the message queue
    void queueInfo(int size, int dropped); ///< Emitted whenever the queue size changes
    void statusOnQueue(int); ///< Emitted when there is a new IGTLinkUSStatusMessage is in the message queue

protected:
    void initializeSonixGrabber();

private slots:
  void receiveFrameSlot(Frame& frame);
  void initializeSonixSlot();

private:
  IGTLinkImageMessage::Pointer convertFrame(Frame& frame);
  IGTLinkUSStatusMessage::Pointer getFrameStatus(Frame& frame);

  void addImageToQueue(IGTLinkImageMessage::Pointer msg); ///< Adds a OpenIGTLink ImageMessage to the queue
  IGTLinkImageMessage::Pointer getLastImageMessageFromQueue(); ///< Gets the oldest message from the queue-

  void addStatusMessageToQueue(IGTLinkUSStatusMessage::Pointer msg); ///< Adds a OpenIGTLink StatusMessage to the queue
  IGTLinkUSStatusMessage::Pointer getLastStatusMessageFromQueue(); ///< Gets the oldest message from the queue-

  std::string createDeviceName();

  QMutex mImageMutex; ///< A lock for making the class threadsafe
  int mMaxqueueInfo;
  std::list<IGTLinkImageMessage::Pointer> mMutexedImageMessageQueue; ///< A threasafe internal queue
  int mDroppedImages;

  QMutex mStatusMutex; ///< A lock for making the class threadsafe
  std::list<IGTLinkUSStatusMessage::Pointer> mMutexedStatusMessageQueue; ///< A threasafe internal queue

  bool mEmitStatusMessage; ///< Emit new US status message
  double mLastFrameTimestamp; ///< Time stamp of last frame
  double mCurrentFrameTimestamp; ///< Current frame timestamp

  vtkSonixVideoSourcePtr  mSonixGrabber; ///< Ultrasonix video grabber
  SonixHelper*          mSonixHelper; ///< Support Qt functionality to vtkSonixVideoSource

};

}//namespace cx

//Declaration of a frame metatype needed to be able to send frame as a signal.
//typedef cx::Frame Frame;
//Q_DECLARE_METATYPE(Frame)

#endif // CX_WIN32

#endif /* CXIMAGESTREAMERSONIX_H_ */
