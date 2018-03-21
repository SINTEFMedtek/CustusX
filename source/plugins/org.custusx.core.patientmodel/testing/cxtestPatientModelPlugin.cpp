/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxPatientModelServiceProxy.h"
#include "cxtestVisServices.h"
#include "cxtestTestDataStructures.h"
#include "cxLogicManager.h"

namespace cxtest
{

TEST_CASE("PatientModelPlugin: makeAvailable works", "[unit][plugins][org.custusx.core.patientmodel]")
{
	TestDataStructures testData;
	cx::ImagePtr image = testData.image1;
	QString imageUid = image->getUid();

	cx::LogicManager::initialize();
	ctkPluginContext* pluginContext = cx::LogicManager::getInstance()->getPluginContext();

	cx::PatientModelServicePtr patientModelService = cx::PatientModelServiceProxy::create(pluginContext);

	patientModelService->insertData(image);
	REQUIRE(patientModelService->getData(imageUid));

	patientModelService->makeAvailable(imageUid, false);
	CHECK(patientModelService->getData(imageUid));//Setting available to false should only remove data from the list, not when asking for a specific uid
	CHECK_FALSE(patientModelService->getDatas().count(imageUid));
	CHECK(patientModelService->getDatas(cx::PatientModelService::AllData).count(imageUid));


	patientModelService->makeAvailable(imageUid, true);
	CHECK(patientModelService->getDatas().count(imageUid));

	cx::LogicManager::shutdown();
}

}//cxtest