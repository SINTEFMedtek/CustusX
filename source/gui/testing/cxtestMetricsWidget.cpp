/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxtestMetricFixture.h"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxSessionStorageService.h"
#include "cxPointMetric.h"
#include <boost/shared_ptr.hpp>
#include <queue>

namespace cxtest
{

class TestMetricManager : public cx::MetricManager
{
//    Q_OBJECT
public:
    TestMetricManager(cx::ViewServicePtr viewService, cx::PatientModelServicePtr patientModelService,
                       cx::TrackingServicePtr trackingService, cx::SpaceProviderPtr spaceProvider) :
        cx::MetricManager(viewService, patientModelService, trackingService, spaceProvider)
    {
    }
    void initWithTestData()
    {
        mUserSettings.coordSys = cx::pcsRAS;
        mUserSettings.imageRefs.push_back("");
        mUserSettings.imageRefs.push_back("");
    }
    void addImage(QString imageRef)
    {
       mUserSettings.imageRefs.push_back(imageRef);
    }
    void setCoordSys(cx::PATIENT_COORDINATE_SYSTEM coordSys)
    {
        mUserSettings.coordSys = coordSys;
    }

};


TEST_CASE("Export and import metrics to and from file", "[integration][metrics][widget]")
{
	cx::LogicManager::initialize();

	MetricFixture fixture;
	fixture.testExportAndImportMetrics();

	cx::LogicManager::shutdown();
}

TEST_CASE("Save the patient and import metrics from the patient XML file", "[integration][metrics][widget]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();

	//scope here to delete the metric manager before shutting down the logic manager.
	{
		QString dataPath = cx::DataLocations::getTestDataPath();
		QString mSession1 = "/temp/TestPatient1.cx3";
		cx::logicManager()->getSessionStorageService()->load(dataPath + mSession1);
        cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(),
                                  cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

		MetricFixture fixture;
		std::vector<cx::DataMetricPtr> metrics = fixture.createMetricsForExport();

		cx::logicManager()->getSessionStorageService()->save();

		foreach (cx::DataMetricPtr metric, metrics)
		{
			cx::logicManager()->getPatientModelService()->removeData(metric->getUid());
		}

		QString patientXMLPath = dataPath + mSession1 + "/custusdoc.xml";
		manager.importMetricsFromXMLFile(patientXMLPath);

		fixture.checkImportedMetricsEqualToExported(metrics, manager);
	}

	cx::LogicManager::shutdown();
}

TEST_CASE("Import metrics from a patient XML file", "[integration][metrics][widget]")
{
	//This test is a regression test of the format of the patient XML file and import of metrics.
	//If the format of the patient file changes, i.e. tag names and tag relations, it might break
	//the metric import.
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();

	//scope here to delete the metric manager before shutting down the logic manager.
	{
		QString dataPath = cx::DataLocations::getTestDataPath();
		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

		MetricFixture fixture;

		int number_of_metrics_before_import = manager.getNumberOfMetrics();
		QString patientXMLPath = dataPath + "/testing/metrics_export_import/baseline_patient_file_with_metrics.xml";
		manager.importMetricsFromXMLFile(patientXMLPath);

		int number_of_metrics_in_file = 6;
		int number_of_metrics_after_import = manager.getNumberOfMetrics();
		CHECK(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));

		CHECK(manager.getNumberOfMetrics() > 0);
	}

	cx::LogicManager::shutdown();
}

TEST_CASE("Import point metrics from MNI Tag Point file", "[integration][metrics]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();

	//scope here to delete the metric manager before shutting down the logic manager.
	{
		QString dataPath = cx::DataLocations::getTestDataPath();
        cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(),
                                  cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

		//MetricFixture fixture;

		int number_of_metrics_before_import = manager.getNumberOfMetrics();
		QString tagFile = dataPath + "/testing/metrics_export_import/Case1-MRI-beforeUS.tag";
		bool testmode = true;
		manager.importMetricsFromMNITagFile(tagFile, testmode);

		int number_of_metrics_in_file = 4;
		int number_of_metrics_after_import = manager.getNumberOfMetrics();
		CHECK(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));
	}

	cx::LogicManager::shutdown();
}

TEST_CASE("Import labeled point metrics from MNI Tag Point file - LPS coordinates", "[unit][metrics]")
{
    cx::LogicManager::initialize();
    cx::DataLocations::setTestMode();
    std::queue<cx::Vector3D> testCoordinates_in_REF_coordinateSystem;
    // Identical coordinate points as specified in file /testing/metrics_export_import/metric_tags_with_labels.tag
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 25.0818, 13.7766));   // A
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 29.5982, 30.9681));   // B
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 35.0026, 50.0849));   // C
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 40.122, 69.659));     // D
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 45.3417, 89.735));    // E
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(21.7821, 7.55754, 80.7703)); // F
    std::queue<QString> labelNames;
    labelNames.push("A");
    labelNames.push("B");
    labelNames.push("C");
    labelNames.push("D");
    labelNames.push("E");
    labelNames.push("F");


    //scope here to delete the metric manager before shutting down the logic manager.
    {
        typedef boost::shared_ptr<class cx::PointMetric> PointMetricPtr;
        QString dataPath = cx::DataLocations::getTestDataPath();
        TestMetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(),
                                  cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

        int number_of_metrics_before_import = manager.getNumberOfMetrics();
        QString tagFile = dataPath + "/testing/metrics_export_import/metric_tags_with_labels.tag";
        bool testmode = true;
        // Manual setting of user dialog settings
        manager.addImage("TestingMetrics");
        manager.setCoordSys(cx::pcsLPS);

        manager.importMetricsFromMNITagFile(tagFile, testmode);

        int number_of_metrics_in_file = 6;
        int number_of_metrics_after_import = manager.getNumberOfMetrics();
        REQUIRE(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));

        std::map<QString, cx::DataPtr>  metrics =
        cx::logicManager()->getPatientModelService()->getDatas();
        std::map<QString, cx::DataPtr>::iterator iter_metrics = metrics.begin();

        REQUIRE(metrics.size() == number_of_metrics_after_import);

        for(; iter_metrics!=metrics.end();++iter_metrics)
        {
            PointMetricPtr  point_metric = boost::static_pointer_cast<cx::PointMetric>(iter_metrics->second);
            cx::Vector3D cxCoord = point_metric->getRefCoord();
            cx::Vector3D testCoord = testCoordinates_in_REF_coordinateSystem.front();
            testCoordinates_in_REF_coordinateSystem.pop();
            REQUIRE(cx::similar(cxCoord, testCoord, 0.1));
            QString cxLabelName = point_metric->getName();
            QString testLabelName = labelNames.front();
            labelNames.pop();
            REQUIRE(QString::compare(cxLabelName,testLabelName) == 0);
        }
    }

    cx::LogicManager::shutdown();
}

TEST_CASE("Import unlabeled point metrics from MNI Tag Point file - LPS coordinates", "[unit][metrics]")
{
    cx::LogicManager::initialize();
    cx::DataLocations::setTestMode();
    std::queue<cx::Vector3D> testCoordinates_in_REF_coordinateSystem;
    // Identical coordinate points as specified in file /testing/metrics_export_import/metric_tags_without_labels.tag
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 25.0818, 13.7766));
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 29.5982, 30.9681));
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 35.0026, 50.0849));
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 40.122, 69.659));
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(25.78, 45.3417, 89.735));
    testCoordinates_in_REF_coordinateSystem.push(cx::Vector3D(21.7821, 7.55754, 80.7703));
    std::queue<QString> labelNames;
    labelNames.push("1");
    labelNames.push("2");
    labelNames.push("3");
    labelNames.push("4");
    labelNames.push("5");
    labelNames.push("6");

    //scope here to delete the metric manager before shutting down the logic manager.
    {
        typedef boost::shared_ptr<class cx::PointMetric> PointMetricPtr;
        QString dataPath = cx::DataLocations::getTestDataPath();
        TestMetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(),
                                  cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

        int number_of_metrics_before_import = manager.getNumberOfMetrics();
        QString tagFile = dataPath + "/testing/metrics_export_import/metric_tags_without_labels.tag";
        bool testmode = true;
        // Manual setting of user dialog settings
        manager.addImage("TestingMetrics");
        manager.setCoordSys(cx::pcsLPS);

        manager.importMetricsFromMNITagFile(tagFile, testmode);

        int number_of_metrics_in_file = 6;
        int number_of_metrics_after_import = manager.getNumberOfMetrics();
        REQUIRE(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));
        // Check coordinates

        std::map<QString, cx::DataPtr>  metrics =
        cx::logicManager()->getPatientModelService()->getDatas();
        std::map<QString, cx::DataPtr>::iterator iter_metrics = metrics.begin();

        REQUIRE(metrics.size() == number_of_metrics_after_import);

        for(; iter_metrics!=metrics.end();++iter_metrics)
        {
            PointMetricPtr  point_metric = boost::static_pointer_cast<cx::PointMetric>(iter_metrics->second);
            cx::Vector3D cxCoord = point_metric->getRefCoord();
            cx::Vector3D testCoord = testCoordinates_in_REF_coordinateSystem.front();
            testCoordinates_in_REF_coordinateSystem.pop();
            REQUIRE(cx::similar(cxCoord, testCoord, 0.1));
            QString cxLabelName = point_metric->getName();
            QString testLabelName = labelNames.front();
            labelNames.pop();
            REQUIRE(QString::compare(cxLabelName,testLabelName) == 0);
        }
    }

    cx::LogicManager::shutdown();
}

TEST_CASE("Import labeled point metrics from MNI Tag Point file - RAS coordinates", "[integration][metrics]")
{
}

} //namespace cxtest
