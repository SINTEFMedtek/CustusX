#include "cxRegistrationMethodManualPatientOrientationService.h"

#include "cxPatientOrientationWidget.h"

namespace cx
{

QWidget *cx::RegistrationMethodManualPatientOrientationService::createWidget()
{
	PatientOrientationWidget* patientOrientationWidget = new PatientOrientationWidget(mServices, NULL, this->getWidgetName(), "Patient Orientation");
	return patientOrientationWidget;
}
} // cx
