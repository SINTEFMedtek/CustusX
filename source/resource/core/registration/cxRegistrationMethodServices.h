#ifndef CXREGISTRATIONMETHODSERVICES_H
#define CXREGISTRATIONMETHODSERVICES_H

#include <boost/shared_ptr.hpp>

namespace cx {

typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;
typedef boost::shared_ptr<class VisualizationService> VisualizationServicePtr;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;

struct regServices
{
	RegistrationServicePtr registrationService;
	PatientModelServicePtr patientModelService;
	VisualizationServicePtr visualizationService;
	TrackingServicePtr trackingService;
};

}

#endif // CXREGISTRATIONMETHODSERVICES_H
