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

#include "cxtestSessionStorageTestFixture.h"

#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxPatientModelServiceProxy.h"

namespace cxtest
{

SessionStorageTestFixture::SessionStorageTestFixture() :
	mSessionsCreated(false)
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	mSessionStorageService = cx::SessionStorageServiceProxy::create(context);
	mPatientModelService = cx::PatientModelServiceProxy::create(context);

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