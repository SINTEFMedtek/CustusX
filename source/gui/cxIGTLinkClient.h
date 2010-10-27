#ifndef CXIGTLINKCLIENT_H_
#define CXIGTLINKCLIENT_H_

#include <vector>
#include <QtCore>
#include "boost/shared_ptr.hpp"

namespace cx
{
typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;

class IGTLinkClient : public QThread
{
public:
  IGTLinkClient(QString address, int port, QObject* parent = NULL);
protected:
  virtual void run();

private:
  QString mAddress;
  int mPort;
};

}//end namespace cx

#endif /* CXIGTLINKCLIENT_H_ */
