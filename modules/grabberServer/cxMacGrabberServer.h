#ifdef __APPLE__
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
 * \\author Janne Beate Bakeng, SINTEF
 */

class MacGrabberServer : public GrabberServer
{

public:
  MacGrabberServer(QObject* parent = NULL);
  virtual ~MacGrabberServer();

protected:
  virtual void connectGrabber(); ///< Connects a MacGrabber.
  virtual void connectServer(); ///< Connects a OpenIGTLinkServer.
};
typedef boost::shared_ptr<MacGrabberServer> MacGrabberServerPtr;
}//namespace cx
#endif /* CXMACGRABBERSERVER_H_ */
#endif /*__APPLE__*/
