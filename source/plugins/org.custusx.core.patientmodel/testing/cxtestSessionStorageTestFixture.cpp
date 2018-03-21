/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestSessionStorageTestFixture.h"

#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxVisServices.h"

namespace cxtest
{

SessionStorageTestFixture::SessionStorageTestFixture() :
	mSessionsCreated(false)
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
    mContext = cx::LogicManager::getInstance()->getPluginContext();
    mSessionStorageService = cx::SessionStorageServiceProxy::create(mContext);
    mPatientModelService = cx::PatientModelServiceProxy::create(mContext);
    mServices = cx::VisServices::create(mContext);

	mSession1 = QString("/temp/TestPatient1.cx3");
	mSession2 = QString("/temp/TestPatient2.cx3");
}

SessionStorageTestFixture::~SessionStorageTestFixture()
{
	cx::LogicManager::shutdown();
}

void SessionStorageTestFixture::createSessions()
{
	if(!mSessionsCreated)
	{
		this->loadSession2();
		this->loadSession1();
	}
}

void SessionStorageTestFixture::saveSession()
{
	mSessionStorageService->save();
}

void SessionStorageTestFixture::loadSession1()
{
	mSessionStorageService->load(cx::DataLocations::getTestDataPath() + mSession1);
}

void SessionStorageTestFixture::loadSession2()
{
	mSessionStorageService->load(cx::DataLocations::getTestDataPath() + mSession2);
}
void SessionStorageTestFixture::reloadSession1()
{
	this->loadSession2();
	this->loadSession1();
}
void SessionStorageTestFixture::reloadSession2()
{
	this->loadSession1();
	this->loadSession2();
}

} //cxtest