#ifndef SSCOPENIGTLINKCLIENT_H_
#define SSCOPENIGTLINKCLIENT_H_

#include <vector>
#include <QtCore>
#include <QTcpSocket>
#include "boost/shared_ptr.hpp"
class QTcpSocket;
#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"

namespace ssc
{
typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;

class IGTLinkClient : public QThread
{
  Q_OBJECT
public:
  IGTLinkClient(QString address, int port, QObject* parent = NULL);
  void stop(); // do not use
  igtl::ImageMessage::Pointer getLastImageMessage();

signals:
  void imageReceived();

protected:
  virtual void run();

private slots:
//  void tick();
  void readyReadSlot();

  void hostFoundSlot();
  void connectedSlot();
  void disconnectedSlot();
  void errorSlot(QAbstractSocket::SocketError);

private:
  bool ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
  void addImageToQueue(igtl::ImageMessage::Pointer imgMsg);

  QString hostDescription() const;
  bool mHeadingReceived;
  bool mStopped;
  QString mAddress;
  int mPort;
  QTcpSocket* mSocket;
//  igtl::ClientSocket::Pointer mSocket;
  igtl::MessageHeader::Pointer mHeaderMsg;

  QMutex mImageMutex;
  std::list<igtl::ImageMessage::Pointer> mMutexedImageMessageQueue;

};

}//end namespace ssc

#endif /* SSCOPENIGTLINKCLIENT_H_ */
