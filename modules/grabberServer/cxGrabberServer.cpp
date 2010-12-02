#include "cxGrabberServer.h"

#include <QMacCocoaViewContainer>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{
GrabberServer::GrabberServer(QObject* parent) :
  QObject(parent),
  mReady(false)
{
  mGrabber = MacGrabberPtr(new MacGrabber());
  connect(mGrabber.get(), SIGNAL(started()), this, SLOT(readySlot()));
  connect(mGrabber.get(), SIGNAL(stopped()), this, SLOT(readySlot()));

  mServer = OpenIGTLinkServerPtr(new OpenIGTLinkServer());
  connect(mServer.get(), SIGNAL(open()), this, SLOT(readySlot()));
  connect(mServer.get(), SIGNAL(closed()), this, SLOT(readySlot()));

  connect(mGrabber.get(), SIGNAL(frame(Frame&)), mServer.get(), SIGNAL(frame(Frame&)), Qt::DirectConnection);
}

GrabberServer::~GrabberServer()
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

QMacCocoaViewContainer* GrabberServer::getPreviewWidget(QWidget* parent)
{
  return mGrabber->getPreviewWidget(parent);
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
  //ssc::messageManager()->sendDebug("Server: "+qstring_cast(mServer->isOpen())+", Grabber: "+qstring_cast(mGrabber->isGrabbing())+", Ready: "+qstring_cast(mReady));
  emit ready(mReady);
}

}//namespace cx
