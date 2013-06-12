#ifndef CXGRABBERSERVER_H_
#define CXGRABBERSERVER_H_

#include <QObject>
#include "cxServer.h"
#include "cxGrabber.h"

namespace cx
{
/**
 * \class GrabberServer
 *
 * \brief Abstract interface that combines and syncs a specific grabber and server.
 *
 * \date 2. des. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class GrabberServer : public QObject
{
  Q_OBJECT

public:
  GrabberServer(QObject* parent = NULL);
  virtual ~GrabberServer(){};

  virtual void start(); ///< Starts both the server and the grabber
  virtual void stop();  ///< Stops both the server and the grabber
  virtual void displayPreview(QWidget* parent); ///< Places a previewwidget into the parent widget for showing the grabbed stream.
  virtual int getPort(); ///< The port the server is listening to.
  virtual void setPort(int port); ///< For changing what port the server should listen to.

signals:
  void ready(bool ready); ///< Emitted when the grabberserver is fully set up
  void queueInfo(int size, int dropped); ///< Emitted whenever the queue size changes

protected slots:
  void readySlot(); ///< Determines when the grabberserver is fully set up

protected:
  virtual void connectGrabber() = 0; ///< Determines what specific grabber is used. Must be implemented by subclasses.
  virtual void connectServer() = 0; ///< Determines what specific server is used. Must be implemented by subclasses.

  GrabberPtr mGrabber; ///< The grabber.
  ServerPtr mServer; ///< The server.
  bool mReady; ///< Fully set up or not.
};

typedef boost::shared_ptr<class GrabberServer> GrabberServerPtr;
}//namespace cx
#endif /* CXGRABBERSERVER_H_ */
