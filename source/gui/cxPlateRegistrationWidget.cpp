#include "cxPlateRegistrationWidget.h"

#include <QPushButton>

namespace cx
{
PlateRegistrationWidget::PlateRegistrationWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mPlateRegistrationButton(new QPushButton("Load registration points", this))
{
  this->setObjectName("PlateRegistrationWidget");
  this->setWindowTitle("Fast Image Registration");

  //connect(mPlateRegistrationButton, SIGNAL(clicked()), );
}

PlateRegistrationWidget::~PlateRegistrationWidget()
{

}

QString PlateRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Plate registration.</h3>"
      "<p>Internally register the reference plates reference points as landmarks.</p>"
      "<p><i>Click register to add landmarks.</i></p>"
      "</html>";
}

}//namespace cx
