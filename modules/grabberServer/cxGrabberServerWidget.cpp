#include "cxGrabberServerWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMacCocoaViewContainer>

namespace cx
{
GrabberServerWidget::GrabberServerWidget(QWidget* parent) :
    QWidget(parent),
    mPreviewParent(new QWidget(this)),
    mStartButton(new QPushButton("Start server", this)),
    mInfoLabel(new QLabel("", this))
{
  mGrabber = new MacGrabber();

  mStartButton->setCheckable(true);

  connect(mStartButton, SIGNAL(clicked()), this, SLOT(startServerSlot()));
  
  //TODO send frames directly to the server
  //connect(mGrabber, SIGNAL(frame()), mServer, SLOT(frameSlot()));

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mPreviewParent);
  layout->addWidget(mStartButton);
  //layout->addWidget(mInfoLabel); //TODO
}

GrabberServerWidget::~GrabberServerWidget()
{
  delete mGrabber;
}

void GrabberServerWidget::startServerSlot()
{
  if(mStartButton->isChecked())
  {
//  mServer->start();
    mGrabber->start();
    mGrabber->getPreviewWidget(mPreviewParent); //TODO change name
    mStartButton->setText("Stop server");
  } else
  {
//  mServer->stop();
    mGrabber->stop();
    mStartButton->setText("Start server");
  }
  //this->updateInfoLabel();
}

void GrabberServerWidget::updateInfoLabel()
{
  //TODO
  //move to statusbar
  /*
  Print info about pixel format, height, width, bytes per row and data size...
  */
}


}//Namespace cx
