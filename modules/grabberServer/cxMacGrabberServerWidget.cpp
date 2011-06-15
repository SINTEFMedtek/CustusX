#ifdef __APPLE__
#include "cxMacGrabberServerWidget.h"

#include <QMacCocoaViewContainer>
#include "cxMacGrabberServer.h"
#include "sscTypeConversions.h"

namespace cx
{

MacGrabberServerWidget::MacGrabberServerWidget(QWidget* parent) :
    GrabberServerWidget(parent)
{
  this->setObjectName("MacGrabberServerWidget");
  this->setWindowTitle("Mac Grabber Server Widget");

  this->connectGrabberServer();
  connect(mGrabberServer.get(), SIGNAL(ready(bool)), this, SLOT(grabberServerReadySlot(bool)));
  this->portChangedSlot(qstring_cast(mGrabberServer->getPort()));
}

void MacGrabberServerWidget::connectGrabberServer()
{
  mGrabberServer = GrabberServerPtr(new MacGrabberServer());
  this->portChangedSlot(qstring_cast(mGrabberServer->getPort()));

  connect(mGrabberServer.get(), SIGNAL(queueInfo(int, int)), this, SIGNAL(queueInfo(int, int)));
}

}//Namespace cx
#endif /*APPLE*/
