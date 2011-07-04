#ifdef WIN32
#include "cxWinGrabberServer.h"

#include "cxWinGrabber.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{

WinGrabberServer::WinGrabberServer(QObject* parent) :
  GrabberServer(parent)
{
  this->connectGrabber();
  this->connectServer();
  connect(mGrabber.get(), SIGNAL(frame(Frame&)), mServer.get(), SIGNAL(frame(Frame&)), Qt::DirectConnection);
}

WinGrabberServer::~WinGrabberServer()
{
}

void WinGrabberServer::connectGrabber()
{
  mGrabber = GrabberPtr(new WinGrabber());
  connect(mGrabber.get(), SIGNAL(started()), this, SLOT(readySlot()));
  connect(mGrabber.get(), SIGNAL(stopped()), this, SLOT(readySlot()));
}

void WinGrabberServer::connectServer()
{
  mServer = ServerPtr(new OpenIGTLinkServer());
  connect(mServer.get(), SIGNAL(open()), this, SLOT(readySlot()));
  connect(mServer.get(), SIGNAL(closed()), this, SLOT(readySlot()));
  connect(mServer.get(), SIGNAL(queueInfo(int, int)), this, SIGNAL(queueInfo(int, int)));
}
}//namespace cx
#endif /*WIN32*/
