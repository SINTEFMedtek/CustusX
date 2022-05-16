/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <queue>
#include <vtkMNITagPointReader.h>
#include <vtkStringArray.h>
#include "cxtestMetricFixture.h"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxSessionStorageService.h"
#include "cxErrorObserver.h"
#include "cxtestMetricManager.h"
#include "cxFileManagerServiceProxy.h"

namespace cxtest
{

namespace
{
struct TestMetricData {
    std::queue<cx::Vector3D> coordinates_in_REF_coordinateSystem;
    std::queue<QString> labelNames;
};

TestMetricData readTestMetricData(QString filename) {
    vtkMNITagPointReaderPtr reader = vtkMNITagPointReader::New();
    reader->SetFileName(filename.toStdString().c_str());
    reader->Update();
    REQUIRE(cx::ErrorObserver::checkedRead(reader, filename));
    QString description(reader->GetComments());
    vtkStringArray *labels = reader->GetLabelText();
    vtkPoints *points = reader->GetPoints(0);
    QString name = "";
    TestMetricData testMetricsfromFile;

    vtkIdType number_of_points = points->GetNumberOfPoints();
    for(vtkIdType i=0; i< number_of_points; ++i)
    {
        vtkStdString label = labels->GetValue(i);
        name = QString::fromStdString(label);
        if(name.isEmpty() || (name == QString(" ")) )   // if no name label is given in .tag file, metric name is set to continous numbering
            name = QString::number(i+1);
        double *point = points->GetPoint(i);
        cx::Vector3D pointVector;
        pointVector[0] = point[0]; pointVector[1] = point[1]; pointVector[2] = point[2];
        testMetricsfromFile.coordinates_in_REF_coordinateSystem.push(pointVector);
        testMetricsfromFile.labelNames.push(name);
    }
    return testMetricsfromFile;
}

}

//Fails on Ubuntu 20.04
TEST_CASE("Export and import metrics to and from file", "[integration][metrics][widget][not_ubuntu2004]")
{
	MetricFixture fixture;
	fixture.testExportAndImportMetrics();
	CHECK_FALSE(fixture.messageListenerContainErrors());
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
		cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider(), filemanager);

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
		
		CHECK_FALSE(fixture.messageListenerContainErrors());
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
		cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider(), filemanager);

		MetricFixture fixture;

		int number_of_metrics_before_import = manager.getNumberOfMetrics();
		QString patientXMLPath = dataPath + "/testing/metrics_export_import/baseline_patient_file_with_metrics.xml";
		manager.importMetricsFromXMLFile(patientXMLPath);

		int number_of_metrics_in_file = 6;
		int number_of_metrics_after_import = manager.getNumberOfMetrics();
		CHECK(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));

		CHECK(manager.getNumberOfMetrics() > 0);
		
		CHECK_FALSE(fixture.messageListenerContainErrors());
	}

	cx::LogicManager::shutdown();
}

//TEST_CASE("Import point metrics from MNI Tag Point file", "[integration][metrics]")
//{
//	cx::LogicManager::initialize();
//	cx::DataLocations::setTestMode();

//	//scope here to delete the metric manager before shutting down the logic manager.
//	{
//		QString dataPath = cx::DataLocations::getTestDataPath();
//		cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
//		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider(), filemanager);

//		//MetricFixture fixture;

//		int number_of_metrics_before_import = manager.getNumberOfMetrics();
//		QString tagFile = dataPath + "/testing/metrics_export_import/Case1-MRI-beforeUS.tag";
//		bool testmode = true;
//		manager.importMetricsFromMNITagFile(tagFile, testmode);

//		int number_of_metrics_in_file = 4;
//		int number_of_metrics_after_import = manager.getNumberOfMetrics();
//		CHECK(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));
//	}

//	cx::LogicManager::shutdown();
//}

//TEST_CASE("Import labeled point metrics from MNI Tag Point file - LPS coordinates", "[integration][metrics]")
//{
//    cx::LogicManager::initialize();
//    cx::DataLocations::setTestMode();
//    QString dataPath = cx::DataLocations::getTestDataPath();
//    QString fileName = QString("/testing/metrics_export_import/metric_tags_with_labels.tag");
//    QString tagFile = dataPath + fileName;
//    TestMetricData metricData = readTestMetricData(tagFile);

//    //scope here to delete the metric manager before shutting down the logic manager.
//    {
//        typedef boost::shared_ptr<class cx::PointMetric> PointMetricPtr;
//        QString dataPath = cx::DataLocations::getTestDataPath();
//        TestMetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(),
//                                  cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

//        int number_of_metrics_before_import = manager.getNumberOfMetrics();
//        QString tagFile = dataPath + fileName;
//        bool testmode = true;
//        // Manual setting of user dialog settings
//        manager.addImage("TestingMetrics");
//        manager.setCoordSys(cx::pcsLPS);

//        manager.importMetricsFromMNITagFile(tagFile, testmode);

//        // Check that metrics are correctly imported
//        int number_of_metrics_in_file = metricData.coordinates_in_REF_coordinateSystem.size();
//        int number_of_metrics_after_import = manager.getNumberOfMetrics();
//        REQUIRE(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));

//        std::map<QString, cx::DataPtr>  metrics =
//        cx::logicManager()->getPatientModelService()->getDatas();
//        std::map<QString, cx::DataPtr>::iterator iter_metrics = metrics.begin();
//        REQUIRE(metrics.size() == number_of_metrics_after_import);

//        // Check that imported coordinates and label names are correct
//        for(; iter_metrics!=metrics.end();++iter_metrics)
//        {
//            PointMetricPtr  point_metric = boost::static_pointer_cast<cx::PointMetric>(iter_metrics->second);
//            cx::Vector3D importedCoordinates = point_metric->getRefCoord();
//            cx::Vector3D spesifiedCoordinates = metricData.coordinates_in_REF_coordinateSystem.front();
//            metricData.coordinates_in_REF_coordinateSystem.pop();
//            REQUIRE(cx::similar(importedCoordinates, spesifiedCoordinates, 0.1));
//            QString cxLabelName = point_metric->getName();
//            QString testLabelName = metricData.labelNames.front();
//            metricData.labelNames.pop();
//            REQUIRE(QString::compare(cxLabelName,testLabelName) == 0);
//        }
//    }

//    cx::LogicManager::shutdown();
//}

//TEST_CASE("Import unlabeled point metrics from MNI Tag Point file - LPS coordinates", "[integration][metrics]")
//{
//    cx::LogicManager::initialize();
//    cx::DataLocations::setTestMode();
//    QString dataPath = cx::DataLocations::getTestDataPath();
//    QString fileName = QString("/testing/metrics_export_import/metric_tags_without_labels.tag");
//    QString tagFile = dataPath + fileName;
//    TestMetricData metricData = readTestMetricData(tagFile);

//    //scope here to delete the metric manager before shutting down the logic manager.
//    {
//        typedef boost::shared_ptr<class cx::PointMetric> PointMetricPtr;
//        QString dataPath = cx::DataLocations::getTestDataPath();
//        TestMetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(),
//                                  cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

//        int number_of_metrics_before_import = manager.getNumberOfMetrics();
//        QString tagFile = dataPath + fileName;
//        bool testmode = true;
//        // Manual setting of user dialog settings
//        manager.addImage("TestingMetrics");
//        manager.setCoordSys(cx::pcsLPS);

//        manager.importMetricsFromMNITagFile(tagFile, testmode);

//        // Check that metrics are correctly imported
//        int number_of_metrics_in_file = metricData.coordinates_in_REF_coordinateSystem.size();
//        int number_of_metrics_after_import = manager.getNumberOfMetrics();
//        REQUIRE(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));

//        std::map<QString, cx::DataPtr>  metrics =
//        cx::logicManager()->getPatientModelService()->getDatas();
//        std::map<QString, cx::DataPtr>::iterator iter_metrics = metrics.begin();
//        REQUIRE(metrics.size() == number_of_metrics_after_import);

//        // Check that imported coordinates and label names are correct
//        for(; iter_metrics!=metrics.end();++iter_metrics)
//        {
//            PointMetricPtr  point_metric = boost::static_pointer_cast<cx::PointMetric>(iter_metrics->second);
//            cx::Vector3D importedCoordinates = point_metric->getRefCoord();
//            cx::Vector3D spesifiedCoordinates = metricData.coordinates_in_REF_coordinateSystem.front();
//            metricData.coordinates_in_REF_coordinateSystem.pop();
//            REQUIRE(cx::similar(importedCoordinates, spesifiedCoordinates, 0.1));
//            QString cxLabelName = point_metric->getName();
//            QString testLabelName = metricData.labelNames.front();
//            metricData.labelNames.pop();
//            REQUIRE(QString::compare(cxLabelName,testLabelName) == 0);
//        }
//    }

//    cx::LogicManager::shutdown();
//}

//TEST_CASE("Import labeled point metrics from MNI Tag Point file - RAS coordinates", "[integration][metrics]")
//{
//    cx::LogicManager::initialize();
//    cx::DataLocations::setTestMode();
//    std::queue<cx::Vector3D> coordinates_RAS_in_REF_coordinateSystem;
//    // Identical coordinate points as specified in file /testing/metrics_export_import/metric_tags_with_labels.tag
//    coordinates_RAS_in_REF_coordinateSystem.push(cx::Vector3D(-25.78, -25.0818, 13.7766));   // A
//    coordinates_RAS_in_REF_coordinateSystem.push(cx::Vector3D(-25.78, -29.5982, 30.9681));   // B
//    coordinates_RAS_in_REF_coordinateSystem.push(cx::Vector3D(-25.78, -35.0026, 50.0849));   // C
//    coordinates_RAS_in_REF_coordinateSystem.push(cx::Vector3D(-25.78, -40.122, 69.659));     // D
//    coordinates_RAS_in_REF_coordinateSystem.push(cx::Vector3D(-25.78, -45.3417, 89.735));    // E
//    coordinates_RAS_in_REF_coordinateSystem.push(cx::Vector3D(-21.7821, -7.55754, 80.7703)); // F
//    std::queue<QString> labelNames;
//    labelNames.push("A");
//    labelNames.push("B");
//    labelNames.push("C");
//    labelNames.push("D");
//    labelNames.push("E");
//    labelNames.push("F");

//    //scope here to delete the metric manager before shutting down the logic manager.
//    {
//        typedef boost::shared_ptr<class cx::PointMetric> PointMetricPtr;
//        QString dataPath = cx::DataLocations::getTestDataPath();
//        TestMetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(),
//                                  cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

//        int number_of_metrics_before_import = manager.getNumberOfMetrics();
//        QString tagFile = dataPath + "/testing/metrics_export_import/metric_tags_with_labels.tag";
//        bool testmode = true;
//        // Manual setting of user dialog settings
//        manager.addImage("TestingMetrics");
//        manager.setCoordSys(cx::pcsRAS);

//        manager.importMetricsFromMNITagFile(tagFile, testmode);

//        // Check that metrics are correctly imported
//        int number_of_metrics_in_file = 6;
//        int number_of_metrics_after_import = manager.getNumberOfMetrics();
//        REQUIRE(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));

//        std::map<QString, cx::DataPtr>  metrics =
//        cx::logicManager()->getPatientModelService()->getDatas();
//        std::map<QString, cx::DataPtr>::iterator iter_metrics = metrics.begin();
//        REQUIRE(metrics.size() == number_of_metrics_after_import);

//        // Check that imported coordinates and label names are correct
//        for(; iter_metrics!=metrics.end();++iter_metrics)
//        {
//            PointMetricPtr  point_metric = boost::static_pointer_cast<cx::PointMetric>(iter_metrics->second);
//            cx::Vector3D importedCoordinates = point_metric->getRefCoord();
//            cx::Vector3D spesifiedCoordinates = coordinates_RAS_in_REF_coordinateSystem.front();
//            coordinates_RAS_in_REF_coordinateSystem.pop();
//            REQUIRE(cx::similar(importedCoordinates, spesifiedCoordinates, 0.1));
//            QString cxLabelName = point_metric->getName();
//            QString testLabelName = labelNames.front();
//            labelNames.pop();
//            REQUIRE(QString::compare(cxLabelName,testLabelName) == 0);
//        }
//    }

//    cx::LogicManager::shutdown();
//}

} //namespace cxtest
