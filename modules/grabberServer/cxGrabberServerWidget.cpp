#include "cxGrabberServerWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

namespace cx
{
GrabberServerWidget::GrabberServerWidget(QWidget* parent) :
    QMacCocoaViewContainer(0, parent),
    mStartButton(new QPushButton("Start server", this)),
    mInfoLabel(new QLabel("", this))
{
  mGrabber = GrabberPtr(new MacGrabber());

  //this->setCocoaView(mObjectiveC->mVideoPreView);

  connect(mStartButton, SIGNAL(clicked()), this, SLOT(startServerSlot()));
  
  //connect(mGrabber, SIGNAL(frame()), mServer, SLOT(frameSlot()));

  //QVBoxLayout* layout = new QVBoxLayout(this);
  //layout->addWidget(mPreviewWidget);
  //layout->addWidget(mStartButton);
  //layout->addWidget(mInfoLabel);
}

GrabberServerWidget::~GrabberServerWidget()
{}

void GrabberServerWidget::startServerSlot()
{
//  mServer->start();
  mGrabber->start();

  this->updateInfoLabel();
}

void GrabberServerWidget::updateInfoLabel()
{}


}//Namespace cx
