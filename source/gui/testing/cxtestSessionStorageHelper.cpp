/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestSessionStorageHelper.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"
#include "cxLogicManager.h"

namespace cxtest
{

void SessionStorageHelper::createTestPatient()
{
	storageFixture.createSessions();
	storageFixture.loadSession1();
}

void SessionStorageHelper::createTestPatientWithData()
{
	createTestPatient();
	storageFixture.mPatientModelService->insertData(testData.mesh1);
	storageFixture.mPatientModelService->insertData(testData.image1);
	storageFixture.mPatientModelService->insertData(testData.image2);
}

cx::VisServicesPtr SessionStorageHelper::getServices()
{
	return cx::VisServices::create(cx::logicManager()->getPluginContext());
}
} //cxtest
