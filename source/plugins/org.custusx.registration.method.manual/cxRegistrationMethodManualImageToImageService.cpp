#include "cxRegistrationMethodManualImageToImageService.h"
#include "cxManualImage2ImageRegistrationWidget.h"


namespace cx
{

RegistrationMethodManualImageToImageService::RegistrationMethodManualImageToImageService(RegistrationServicePtr registrationService) :
	RegistrationMethodManualService(registrationService)
{
}

QWidget* RegistrationMethodManualImageToImageService::createWidget()
{
	QWidget* retval = new ManualImage2ImageRegistrationWidget(mRegistrationService, NULL, this->getWidgetName());
	return retval;
}

} //cx
