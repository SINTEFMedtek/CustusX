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

#include "cxPatientModelServiceProxy.h"
#include "cxtestVisServices.h"
#include "cxtestTestDataStructures.h"
#include "cxLogicManager.h"

namespace cxtest
{

TEST_CASE("PatientModelPlugin: Make unavailable works", "[unit][plugins][org.custusx.core.patientmodel]")
{
	TestDataStructures testData;
	cx::ImagePtr image = testData.image1;
	QString imageUid = image->getUid();

	cx::LogicManager::initialize();
	ctkPluginContext* pluginContext = cx::LogicManager::getInstance()->getPluginContext();

	cx::PatientModelServicePtr patientModelService = cx::PatientModelServiceProxy::create(pluginContext);

	patientModelService->insertData(image);
	REQUIRE(patientModelService->getData(imageUid));

	patientModelService->makeUnavailable(imageUid);
	CHECK_FALSE(patientModelService->getData(imageUid));
	CHECK_FALSE(patientModelService->getData().count(imageUid));

	cx::LogicManager::shutdown();
}

}//cxtest