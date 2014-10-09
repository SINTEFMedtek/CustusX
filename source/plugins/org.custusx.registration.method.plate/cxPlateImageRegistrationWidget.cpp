#include "cxPlateImageRegistrationWidget.h"
#include "cxPatientModelService.h"

namespace cx {

PlateImageRegistrationWidget::PlateImageRegistrationWidget(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService, QWidget* parent) :
	FastImageRegistrationWidget(registrationService, patientModelService, parent, "PlateImageRegistrationWidget", "Plate Image Registration"),
	mPatientModelService(patientModelService)
{
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

void PlateImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
	mPatientModelService->setLandmarkActive(mActiveLandmark, true);
	LandmarkImageRegistrationWidget::editLandmarkButtonClickedSlot();
}

void PlateImageRegistrationWidget::performRegistration()
{
	FastImageRegistrationWidget::performRegistration();
	mRegistrationService->doFastRegistration_Translation();
}

} //cx
