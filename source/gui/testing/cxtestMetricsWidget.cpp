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


#include "cxPointMetric.h"
#include "cxtestSpaceProviderMock.h"
#include "cxMetricManager.h"
#include "cxPatientModelService.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"

//#include "cxMetricFileReader.h"


namespace cxtest
{


TEST_CASE("Export and import metrics to and from file", "[unit][gui][jon]")
{
	MetricFixture fixture;
	fixture.testExportAndImportMetrics();

//	cx::DataLocations::setTestMode();
//	cx::LogicManager::initialize();
//	cx::MetricManager manager;

//    QString uid = "point1";
//    cx::PointMetricPtr point = cx::PointMetric::create(uid, "", cx::PatientModelServicePtr(), cxtest::SpaceProviderMock::create());
//    point->setSpace(cx::CoordinateSystem(cx::csTOOL));
//    point->setCoordinate(cx::Vector3D(1,2,3));
//    cx::patientService()->insertData(point);
//    QString p1_str = point->getAsSingleLineString();

//	QString metricsFilePath = cx::DataLocations::getTestDataPath() + "/testing/export_and_import_metrics_test_file.txt";
//	manager.exportMetricsToFile(metricsFilePath);

//    manager.importMetricsFromFile(metricsFilePath);

//    cx::DataMetricPtr p2 = manager.getMetric("point2");
//    CHECK(p1_str == p2->getAsSingleLineString());

//	p2.reset(); //have to reset this before shutdown, or cxSharedPointerChecker.h comes into effect.
//    cx::LogicManager::shutdown();
}

} //namespace cxtest
