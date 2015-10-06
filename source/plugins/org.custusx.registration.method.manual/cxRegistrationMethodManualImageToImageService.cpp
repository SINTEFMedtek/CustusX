#include "cxRegistrationMethodManualImageToImageService.h"
#include "cxManualImage2ImageRegistrationWidget.h"


namespace cx
{

RegistrationMethodManualImageToImageService::RegistrationMethodManualImageToImageService(RegServicesPtr services) :
	RegistrationMethodManualService(services)
{
}

QWidget* RegistrationMethodManualImageToImageService::createWidget()
{
	QWidget* retval = new ManualImage2ImageRegistrationWidget(mServices, NULL, this->getWidgetName());
	return retval;
}

} //cx
