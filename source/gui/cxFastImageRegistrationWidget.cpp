#include "cxFastImageRegistrationWidget.h"

#include "sscMessageManager.h"
#include "cxRegistrationManager.h"

namespace cx
{

FastImageRegistrationWidget::FastImageRegistrationWidget(QWidget* parent) :
    ImageRegistrationWidget(parent)
{}

FastImageRegistrationWidget::~FastImageRegistrationWidget()
{

}

void FastImageRegistrationWidget::performRegistration()
{
  //ssc::messageManager()->sendError("Fast image registration is not implemented yet!!!");
  this->updateAvarageAccuracyLabel();
}

}//namespace cx
