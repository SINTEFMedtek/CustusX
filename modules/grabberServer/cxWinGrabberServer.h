#ifdef WIN32
#ifndef CXWINGRABBERSERVER_H_
#define CXWINGRABBERSERVER_H_

#include"cxGrabberServer.h"

namespace cx
{
/**
 * \class WinGrabberServer
 *
 * \brief Combines and syncs a WinGraber and a OpenIGTLinkServer
 *
 * \date  06. jun. 2011
 * \\author Janne Beate Bakeng, SINTEF
 */

class WinGrabberServer : public GrabberServer
{
  Q_OBJECT

public:
  WinGrabberServer(QObject* parent = NULL);
  virtual ~WinGrabberServer();

protected:
  virtual void connectGrabber(); ///< Connects a WinGrabber.
  virtual void connectServer(); ///< Connects a OpenIGTLinkServer.
};
typedef boost::shared_ptr<class WinGrabberServer> WinGrabberServerPtr;
}//namespace cx
#endif /* CXWINGRABBERSERVER_H_ */
#endif /*WIN32*/
