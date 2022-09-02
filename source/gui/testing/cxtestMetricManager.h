#ifndef CXTESTMETRICMANAGER_H
#define CXTESTMETRICMANAGER_H

#include "cxtestgui_export.h"

#include "cxMetricManager.h"
#include "cxFileManagerServiceProxy.h"


namespace cxtest
{


class CXTESTGUI_EXPORT TestMetricManager : public cx::MetricManager
{

public:
    TestMetricManager(cx::ViewServicePtr viewService, cx::PatientModelServicePtr patientModelService,
                       cx::TrackingServicePtr trackingService, cx::SpaceProviderPtr spaceProvider, cx::FileManagerServicePtr filemanager);

    void initWithTestData();
    void addImage(QString imageRef);
    void setCoordSys(cx::PATIENT_COORDINATE_SYSTEM coordSys);
};

} // namespace cxtest

#endif // CXTESTMETRICMANAGER_H
