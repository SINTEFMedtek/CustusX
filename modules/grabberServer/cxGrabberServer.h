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
  virtual ~GrabberServer(){};

  virtual void start();
  virtual void stop();
  virtual void displayPreview(QWidget* parent);
  virtual int getPort();
  virtual void setPort(int port);

signals:
  void ready(bool ready);

protected slots:
  void readySlot();

protected:
  virtual void connectGrabber() = 0;
  virtual void connectServer() = 0;

  GrabberPtr mGrabber;
  ServerPtr mServer;
  bool mReady;
};

/**
 * \class MacGrabberServer
 *
 * \brief
 *
 * \date 2. des. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

class MacGrabberServer : public GrabberServer
{
  Q_OBJECT

public:
  MacGrabberServer(QObject* parent = NULL);
  virtual ~MacGrabberServer();

protected:
  virtual void connectGrabber();
  virtual void connectServer();
};

typedef boost::shared_ptr<class GrabberServer> GrabberServerPtr;
typedef boost::shared_ptr<class MacGrabberServer> MacGrabberServerPtr;
}//namespace cx
#endif /* CXGRABBERSERVER_H_ */
