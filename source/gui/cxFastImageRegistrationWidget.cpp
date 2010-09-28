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

QString FastImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Fast image registration.</h3>"
      "<p>Select landmarks in the data set that you want to use for performing fast registration.</p>"
      "<p><i>Click in the dataset and push the add or resample button.</i></p>"
      "</html>";
}

void FastImageRegistrationWidget::performRegistration()
{
  //make sure the masterImage is set
  ssc::ImagePtr masterImage = registrationManager()->getMasterImage();
  if(!masterImage)
    registrationManager()->setMasterImage(mCurrentImage);

  this->updateAvarageAccuracyLabel();
}

}//namespace cx
