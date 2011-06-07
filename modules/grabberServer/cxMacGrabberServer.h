#ifdef(APPLE)
#ifndef CXMACGRABBERSERVER_H_
#define CXMACGRABBERSERVER_H_

#include"cxGrabberServer.h"

class QMacCocoaViewContainer;

namespace cx
{
/**
 * \class MacGrabberServer
 *
 * \brief Combines and syns a MacGrabber and a OpenIGTLinkServer
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
  virtual void connectGrabber(); ///< Connects a MacGrabber.
  virtual void connectServer(); ///< Connects a OpenIGTLinkServer.
};
typedef boost::shared_ptr<class MacGrabberServer> MacGrabberServerPtr;
}//namespace cx
#endif /* CXMACGRABBERSERVER_H_ */
#endif(APPLE)
