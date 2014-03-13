#include "cxFastPatientRegistrationWidget.h"

#include "cxReporter.h"
#include "cxRegistrationManager.h"

namespace cx
{
FastPatientRegistrationWidget::FastPatientRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    LandmarkPatientRegistrationWidget(regManager, parent, "FastPatientRegistrationWidget", "Fast Patient Registration")
{
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
  mManager->doFastRegistration_Translation();
  this->updateAvarageAccuracyLabel();
}

}//namespace cx
