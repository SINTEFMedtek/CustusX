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

namespace cxtest
{


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
		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

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
		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider());

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



} //namespace cxtest
