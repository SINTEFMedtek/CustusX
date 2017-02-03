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

#include "cxPointMetric.h"
#include "cxtestSpaceProviderMock.h"
#include "cxMetricManager.h"

#include "cxPatientModelService.h"
//#include "cxCoreServices.h" //trengs ikke antakelig

#include "cxDataLocations.h"
#include "cxLogicManager.h"

namespace cxtest
{

TEST_CASE("Export and import metrics to and from file", "[unit][gui]")
{
    cx::DataLocations::setTestMode();
    cx::LogicManager::initialize();

    QString uid = "Point1";
    cx::PointMetricPtr point = cx::PointMetric::create(uid, "", cx::PatientModelServicePtr(), cxtest::SpaceProviderMock::create());
    cx::patientService()->insertData(point);

    cx::MetricManager manager;
    std::string uid_1 = manager.getMetric(uid)->getUid().toStdString();
    CHECK(manager.getMetric(uid)->getUid().toStdString() == uid.toStdString());


    QString metricsFilePath = cx::DataLocations::getTestDataPath() + "/testing/metrics.txt";
    manager.exportMetricsToFile(metricsFilePath);
    cx::patientService()->removeData(uid);

    manager.importMetricsFromFile(metricsFilePath);

    CHECK(manager.getMetric(uid));


    cx::LogicManager::shutdown();
}

} //namespace cxtest
