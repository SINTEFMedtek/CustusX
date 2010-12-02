#include "cxGrabberServerWidget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMacCocoaViewContainer>
#include "sscTypeConversions.h"

namespace cx
{
GrabberServerWidget::GrabberServerWidget(QWidget* parent) :
    QWidget(parent),
    mPreviewParent(new QWidget(this)),
    mStartButton(new QPushButton("Start server", this)),
    mPortEdit(new QLineEdit())

{
  mGrabberServer = GrabberServerPtr(new GrabberServer());
  connect(mGrabberServer.get(), SIGNAL(ready(bool)), this, SLOT(serverReadySlot(bool)));

  mPortEdit->setText(qstring_cast(mGrabberServer->getPort()));
  connect(mPortEdit, SIGNAL(textChanged(const QString&)), this, SLOT(portChangedSlot(const QString&)));

  mStartButton->setCheckable(true);
  connect(mStartButton, SIGNAL(clicked(bool)), this, SLOT(startServerSlot(bool)));

  QVBoxLayout* layout = new QVBoxLayout(this);
  QGridLayout* gridLayout = new QGridLayout();
  layout->addWidget(mPreviewParent);
  layout->addLayout(gridLayout);
  gridLayout->addWidget(new QLabel("Port: "), 0, 0);
  gridLayout->addWidget(mPortEdit, 0, 1);
  gridLayout->addWidget(mStartButton, 0, 2);
}

GrabberServerWidget::~GrabberServerWidget()
{}

void GrabberServerWidget::startServerSlot(bool start)
{
  mStartButton->setChecked(false);

  if(start)
    mGrabberServer->start();
  else
    mGrabberServer->stop();
}

void GrabberServerWidget::portChangedSlot(const QString& port)
{
  bool ok;
  int newPort = port.toInt(&ok);
  if(!ok)
    return;

  if(mGrabberServer->getPort() == newPort)
    return;

  mGrabberServer->setPort(newPort);
}

void GrabberServerWidget::serverReadySlot(bool ready)
{
  if(ready)
  {
    mGrabberServer->getPreviewWidget(mPreviewParent);
    mStartButton->setText("Stop server");
    mStartButton->setChecked(true);
  }
  else
  {
    mStartButton->setText("Start server");
    mStartButton->setChecked(false);
  }
}

}//Namespace cx
