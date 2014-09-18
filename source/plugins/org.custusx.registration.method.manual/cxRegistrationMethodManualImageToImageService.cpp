#include "cxRegistrationMethodManualImageToImageService.h"
#include "cxManualImage2ImageRegistrationWidget.h"


namespace cx
{

RegistrationMethodManualImageToImageService::RegistrationMethodManualImageToImageService(ctkPluginContext *context) :
	RegistrationMethodManualService(context)
{
}

QWidget* RegistrationMethodManualImageToImageService::createWidget()
{
	QWidget* retval = new ManualImage2ImageRegistrationWidget(mPluginContext, NULL, this->getWidgetName());
	return retval;
}

} //cx
