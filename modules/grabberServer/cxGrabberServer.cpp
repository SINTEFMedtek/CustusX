#include "cxGrabberServer.h"

#include <QMacCocoaViewContainer>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{

//=============================================================================
GrabberServer::GrabberServer(QObject* parent) :
    QObject(parent),
    mReady(false)
{
}
void GrabberServer::start()
{
  mServer->start();
  mGrabber->start();
}

void GrabberServer::stop()
{
  mGrabber->stop();
  mServer->stop();
}

void GrabberServer::displayPreview(QWidget* parent)
{
  mGrabber->displayPreview(parent);
}

int GrabberServer::getPort()
{
  return mServer->getPort();
}

void GrabberServer::setPort(int port)
{
  mServer->setPort(port);
}

void GrabberServer::readySlot()
{
  mReady =  mServer->isOpen() && mGrabber->isGrabbing();
  emit ready(mReady);
}


//=============================================================================

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
  connect(mServer.get(), SIGNAL(queueSize(int)), this, SIGNAL(queueSize(int)));
}


}//namespace cx
