#include "cxFastPatientRegistrationWidget.h"

#include "sscMessageManager.h"
#include "cxRegistrationManager.h"

namespace cx
{
FastPatientRegistrationWidget::FastPatientRegistrationWidget(QWidget* parent) :
    PatientRegistrationWidget(parent)
{}

FastPatientRegistrationWidget::~FastPatientRegistrationWidget()
{}

void FastPatientRegistrationWidget::performRegistration()
{
  //ssc::messageManager()->sendError("Fast patient registration is not implemented yet!!!");
  registrationManager()->doFastRegistration_Translation();
  this->updateAvarageAccuracyLabel();
}

}//namespace cx
