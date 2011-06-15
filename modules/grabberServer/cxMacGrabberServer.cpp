#ifdef __APPLE__
#include "cxMacGrabberServer.h"

#include <QMacCocoaViewContainer>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxMacGrabber.h"

namespace cx
{

MacGrabberServer::MacGrabberServer(QObject* parent) :
  GrabberServer(parent)
{
  this->connectGrabber();
  this->connectServer();
  connect(mGrabber.get(), SIGNAL(frame(Frame&)), mServer.get(), SIGNAL(frame(Frame&)), Qt::DirectConnection);
}

MacGrabberServer::~MacGrabberServer()
{
}

void MacGrabberServer::connectGrabber()
{
  mGrabber = GrabberPtr(new MacGrabber());
  connect(mGrabber.get(), SIGNAL(started()), this, SLOT(readySlot()));
  connect(mGrabber.get(), SIGNAL(stopped()), this, SLOT(readySlot()));
}

void MacGrabberServer::connectServer()
{
  mServer = ServerPtr(new OpenIGTLinkServer());
  connect(mServer.get(), SIGNAL(open()), this, SLOT(readySlot()));
  connect(mServer.get(), SIGNAL(closed()), this, SLOT(readySlot()));
  connect(mServer.get(), SIGNAL(queueInfo(int, int)), this, SIGNAL(queueInfo(int, int)));
}
}//namespace cx
#endif /*APPLE*/
