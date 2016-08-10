#ifndef CXMETRICUTILITIES_H
#define CXMETRICUTILITIES_H

#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxDataMetricWrappers.h"

namespace cx
{

class MetricUtilities
{
public:
	MetricUtilities(ViewServicePtr viewService, PatientModelServicePtr patientModelService);

	MetricBasePtr createMetricWrapper(DataPtr data);
	std::vector<MetricBasePtr> createMetricWrappers();

private:
	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
};

}//cx

#endif // CXMETRICUTILITIES_H
