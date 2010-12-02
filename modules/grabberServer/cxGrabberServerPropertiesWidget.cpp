#include "cxGrabberServerPropertiesWidget.h"

#include <QGridLayout>
#include <QLabel>

namespace cx
{
GrabberServerPropertiesWidget::GrabberServerPropertiesWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("GrabberServerPropertiesWidget");
  this->setWindowTitle("Properties");

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  mLayout = new QGridLayout();

  topLayout->addLayout(mLayout);

  mLayout->addWidget(new QLabel(""));
}

GrabberServerPropertiesWidget::~GrabberServerPropertiesWidget()
{}

void GrabberServerPropertiesWidget::setGrabber(MacGrabberPtr grabber)
{
  mGrabber = grabber;
}

void GrabberServerPropertiesWidget::setServer(OpenIGTLinkServerPtr server)
{
  mServer = server;
}
}//namespace cx
