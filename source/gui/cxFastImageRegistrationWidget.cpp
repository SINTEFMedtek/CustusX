#include "cxFastImageRegistrationWidget.h"

#include "sscMessageManager.h"
#include "cxRegistrationManager.h"

namespace cx
{

FastImageRegistrationWidget::FastImageRegistrationWidget(QWidget* parent) :
    ImageRegistrationWidget(parent)
{
  this->setObjectName("FastImageRegistrationWidget");
  this->setWindowTitle("Fast Image Registration");
}

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
  ssc::DataPtr fixedData = registrationManager()->getFixedData();
  if(!fixedData)
    registrationManager()->setFixedData(mCurrentImage);

  this->updateAvarageAccuracyLabel();
}
//------------------------------------------------------------------------------
PlateImageRegistrationWidget::PlateImageRegistrationWidget(QWidget* parent) :
    FastImageRegistrationWidget(parent)
{
  this->setObjectName("FastImageRegistrationWidget");
  this->setWindowTitle("Fast Image Registration");
}

PlateImageRegistrationWidget::~PlateImageRegistrationWidget()
{}

QString PlateImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Image registration.</h3>"
      "<p>Select landmarks in the data set that you want to use for performing plate registration.</p>"
      "<p><i>Click in the dataset and push the add or resample button.</i></p>"
      "</html>";
}

void PlateImageRegistrationWidget::performRegistration()
{
  FastImageRegistrationWidget::performRegistration();
  registrationManager()->doFastRegistration_Translation();
  ssc::messageManager()->sendDebug("Fast translation registration requested.");
}
//------------------------------------------------------------------------------

}//namespace cx
