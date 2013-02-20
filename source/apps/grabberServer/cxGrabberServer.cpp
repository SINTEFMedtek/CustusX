#include "cxGrabberServer.h"

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

}//namespace cx
