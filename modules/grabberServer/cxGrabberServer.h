#ifndef CXGRABBERSERVER_H_
#define CXGRABBERSERVER_H_

#include <QObject>
#include "cxServer.h"
#include "cxGrabber.h"

class QMacCocoaViewContainer;

namespace cx
{
/**
 * \class GrabberServer
 *
 * \brief
 *
 * \date 2. des. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

class GrabberServer : public QObject
{
  Q_OBJECT

public:
  GrabberServer(QObject* parent = NULL);
  ~GrabberServer();

  void start();
  void stop();

  QMacCocoaViewContainer* getPreviewWidget(QWidget* parent);
  int getPort();
  void setPort(int port);

signals:
  void ready(bool ready);

private slots:
  void readySlot();

private:
  MacGrabberPtr mGrabber;
  OpenIGTLinkServerPtr mServer;

  bool mReady;
};

typedef boost::shared_ptr<class GrabberServer> GrabberServerPtr;
}//namespace cx
#endif /* CXGRABBERSERVER_H_ */
