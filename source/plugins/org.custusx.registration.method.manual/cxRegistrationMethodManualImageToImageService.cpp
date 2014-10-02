#include "cxRegistrationMethodManualImageToImageService.h"
#include "cxManualImage2ImageRegistrationWidget.h"


namespace cx
{

RegistrationMethodManualImageToImageService::RegistrationMethodManualImageToImageService(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService) :
	RegistrationMethodManualService(registrationService),
	mPatientModelService(patientModelService)
{
}

QWidget* RegistrationMethodManualImageToImageService::createWidget()
{
	QWidget* retval = new ManualImage2ImageRegistrationWidget(mRegistrationService, mPatientModelService, NULL, this->getWidgetName());
	return retval;
}

} //cx
