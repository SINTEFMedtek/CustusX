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
  mGrabber = MacGrabberPtr(new MacGrabber());
  mServer = OpenIGTLinkServerPtr(new OpenIGTLinkServer());

  mPortEdit->setText(qstring_cast(mServer->getPort()));
  connect(mPortEdit, SIGNAL(textChanged(const QString&)), this, SLOT(portChangedSlot(const QString&)));

  mStartButton->setCheckable(true);

  connect(mStartButton, SIGNAL(clicked()), this, SLOT(startServerSlot()));
  
  connect(mGrabber.get(), SIGNAL(frame(Frame&)), mServer.get(), SIGNAL(frame(Frame&)), Qt::DirectConnection);

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

MacGrabberPtr GrabberServerWidget::getGrabber()
{
  return mGrabber;
}

OpenIGTLinkServerPtr GrabberServerWidget::getServer()
{
  return mServer;
}

void GrabberServerWidget::startServerSlot()
{
  if(mStartButton->isChecked())
  {
    mServer->start();
    mGrabber->start();
    mGrabber->getPreviewWidget(mPreviewParent); //TODO change name
    mStartButton->setText("Stop server");
  } else
  {
    mServer->stop();
    mGrabber->stop();
    mStartButton->setText("Start server");
  }
}

void GrabberServerWidget::portChangedSlot(const QString& port)
{
  bool ok;
  int newPort = port.toInt(&ok);
  if(!ok)
    return;

  if(mServer->getPort() == newPort)
    return;

  mServer->setPort(newPort);
}

}//Namespace cx
