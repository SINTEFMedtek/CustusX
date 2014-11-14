#include "cxRegistrationMethodServices.h"

#include <ctkPluginContext.h>
#include "cxRegistrationServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxVisualizationServiceProxy.h"
#include "cxTrackingServiceProxy.h"

namespace cx {

RegServices::RegServices(ctkPluginContext* context)
{
	registrationService	= RegistrationServicePtr(new RegistrationServiceProxy(context));
	patientModelService	= PatientModelServicePtr(new PatientModelServiceProxy(context));
	visualizationService = VisualizationServicePtr(new VisualizationServiceProxy(context));
	trackingService		= TrackingServicePtr(new TrackingServiceProxy(context));
}

RegServices RegServices::getNullObjects()
{
//	static regServices mServices();
//	return mServices();
	return RegServices();
}

RegServices::RegServices()
{
	registrationService	= cx::RegistrationService::getNullObject();
	patientModelService	= cx::PatientModelService::getNullObject();
	visualizationService = cx::VisualizationService::getNullObject();
	trackingService	= cx::TrackingService::getNullObject();
}
} // cx
