#ifdef WIN32
#include "cxWinGrabberServerWidget.h"

#include "sscTypeConversions.h"
#include "cxWinGrabberServer.h"

namespace cx
{

WinGrabberServerWidget::WinGrabberServerWidget(QWidget* parent) :
    GrabberServerWidget(parent)
{
  this->setObjectName("WinGrabberServerWidget");
  this->setWindowTitle("Win Grabber Server Widget");

  this->connectGrabberServer();
  connect(mGrabberServer.get(), SIGNAL(ready(bool)), this, SLOT(grabberServerReadySlot(bool)));
  this->portChangedSlot(qstring_cast(mGrabberServer->getPort()));
}

void WinGrabberServerWidget::connectGrabberServer()
{
  mGrabberServer = GrabberServerPtr(new WinGrabberServer());
  this->portChangedSlot(qstring_cast(mGrabberServer->getPort()));

  connect(mGrabberServer.get(), SIGNAL(queueInfo(int, int)), this, SIGNAL(queueInfo(int, int)));
}

}//Namespace cx
#endif /*WIN32*/
