#include "cxtestMetricManager.h"

namespace cxtest
{
TestMetricManager::TestMetricManager(cx::ViewServicePtr viewService, cx::PatientModelServicePtr patientModelService,
                   cx::TrackingServicePtr trackingService, cx::SpaceProviderPtr spaceProvider, cx::FileManagerServicePtr filemanager) :
    cx::MetricManager(viewService, patientModelService, trackingService, spaceProvider, filemanager)
{
}

void TestMetricManager::initWithTestData()
{
    mUserSettings.coordSys = cx::pcsRAS;
    mUserSettings.imageRefs.push_back("");
    mUserSettings.imageRefs.push_back("");
}

void TestMetricManager::addImage(QString imageRef)
{
   mUserSettings.imageRefs.push_back(imageRef);
}

void TestMetricManager::setCoordSys(cx::PATIENT_COORDINATE_SYSTEM coordSys)
{
    mUserSettings.coordSys = coordSys;
}

} // namespace cxtest
