/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider(), cx::logicManager()->getFileManagerService());

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
		cx::MetricManager manager(cx::logicManager()->getViewService(), cx::logicManager()->getPatientModelService(), cx::logicManager()->getTrackingService(), cx::logicManager()->getSpaceProvider(), cx::logicManager()->getFileManagerService());

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





} //namespace cxtest
