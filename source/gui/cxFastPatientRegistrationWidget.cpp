#include "cxFastPatientRegistrationWidget.h"

#include "sscMessageManager.h"
#include "cxRegistrationManager.h"

namespace cx
{
FastPatientRegistrationWidget::FastPatientRegistrationWidget(QWidget* parent) :
    PatientRegistrationWidget(parent)
{
  this->setObjectName("FastPatientRegistrationWidget");
  this->setWindowTitle("Fast Patient Registration");
}

FastPatientRegistrationWidget::~FastPatientRegistrationWidget()
{}

QString FastPatientRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Fast translation registration. </h3>"
      "<p>Select landmarks on the patient that corresponds to one or more of the points sampled in image registration. "
      "Points are used to determine the translation of the patient registration.</p>"
      "<p><i>Point on the patient using a tool and click the Sample button.</i></p>"
      "</html>";
}

void FastPatientRegistrationWidget::performRegistration()
{
  registrationManager()->doFastRegistration_Translation();
  this->updateAvarageAccuracyLabel();
}

}//namespace cx
