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
  //make sure the masterImage is set
  ssc::ImagePtr masterImage = registrationManager()->getMasterImage();
  if(!masterImage)
    registrationManager()->setMasterImage(mCurrentImage);

  //ssc::messageManager()->sendError("Fast image registration is not implemented yet!!!");
  this->updateAvarageAccuracyLabel();
}

}//namespace cx
