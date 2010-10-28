#ifndef CXIGTLINKCLIENT_H_
#define CXIGTLINKCLIENT_H_

#include <vector>
#include <QtCore>
#include "boost/shared_ptr.hpp"

#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"

namespace cx
{
typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;

class IGTLinkClient : public QThread
{
  Q_OBJECT
public:
  IGTLinkClient(QString address, int port, QObject* parent = NULL);
  void stop();

protected:
  virtual void run();

private slots:
  void tick();

private:
  bool mStopped;
  QString mAddress;
  int mPort;
  igtl::ClientSocket::Pointer mSocket;
  igtl::MessageHeader::Pointer mHeaderMsg;
};

}//end namespace cx

#endif /* CXIGTLINKCLIENT_H_ */
