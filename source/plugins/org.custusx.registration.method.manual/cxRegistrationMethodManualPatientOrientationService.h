#ifndef CXREGISTRATIONMETHODMANUALPATIENTORIENTATIONSERVICE_H
#define CXREGISTRATIONMETHODMANUALPATIENTORIENTATIONSERVICE_H


#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_manual_Export.h"

namespace cx
{

/**
 * Registration method: Patient orientation service implementation
 *
 * \ingroup org_custusx_registration_method_manual
 *
 * \date 2014-10-06
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_manual_EXPORT RegistrationMethodManualPatientOrientationService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodManualPatientOrientationService(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService) :
		RegistrationMethodService(registrationService),
		mPatientModelService(patientModelService) {}
	virtual ~RegistrationMethodManualPatientOrientationService() {}
	virtual QString getRegistrationType() {return QString("ImageTransform");}
	virtual QString getRegistrationMethod() {return QString("Manual Patient Orientation");}
	virtual QString getWidgetName() {return QString("ManualPatientOrientationRegistrationWidget");}
	virtual QWidget* createWidget();

private:
	PatientModelServicePtr mPatientModelService;
};
} // cx

#endif // CXREGISTRATIONMETHODMANUALPATIENTORIENTATIONSERVICE_H
