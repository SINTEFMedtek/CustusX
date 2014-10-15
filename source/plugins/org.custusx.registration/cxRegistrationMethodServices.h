#ifndef CXREGISTRATIONMETHODSERVICES_H
#define CXREGISTRATIONMETHODSERVICES_H

#include "org_custusx_registration_Export.h"
#include <boost/shared_ptr.hpp>
class ctkPluginContext;

namespace cx
{

typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;
typedef boost::shared_ptr<class VisualizationService> VisualizationServicePtr;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;

class org_custusx_registration_EXPORT regServices
{
public:
	regServices(ctkPluginContext* context);
	static regServices getNullObjects();

	RegistrationServicePtr registrationService;
	PatientModelServicePtr patientModelService;
	VisualizationServicePtr visualizationService;
	TrackingServicePtr trackingService;
private:
	regServices();
};

}

#endif // CXREGISTRATIONMETHODSERVICES_H
