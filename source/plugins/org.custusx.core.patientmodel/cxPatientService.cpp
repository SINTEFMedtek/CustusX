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

#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxFileHelpers.h"
#include "cxDataLocations.h"
#include "cxTypeConversions.h"
#include "cxDataManagerImpl.h"

namespace cx
{

//// --------------------------------------------------------
//PatientService* PatientService::mInstance = NULL; ///< static member
//// --------------------------------------------------------

PatientServicePtr PatientService::create(DataServicePtr dataService)
{
	PatientServicePtr retval;
	retval.reset(new PatientService(dataService));
//	retval->mSelf = retval;
	return retval;
}

//void PatientService::initialize()
//{
//	PatientService::getInstance();
//}

//void PatientService::shutdown()
//{
//	delete mInstance;
//	mInstance = NULL;
//}

//PatientService* PatientService::getInstance()
//{
//	if (!mInstance)
//	{
//		PatientService::setInstance(new PatientService());
//	}
//	return mInstance;
//}

//void PatientService::setInstance(PatientService* instance)
//{
//	if (mInstance)
//	{
//		delete mInstance;
//	}
//	mInstance = instance;
//}

void PatientService::clearCache()
{
//	std::cout << "DataLocations::getCachePath() " << DataLocations::getCachePath() << std::endl;
	// clear the global cache used by app
	removeNonemptyDirRecursively(DataLocations::getCachePath());
}

PatientService::PatientService(DataServicePtr dataService)
{
	this->clearCache();
	mDataService = dataService;
//	mDataService = DataManagerImpl::create();
	mPatientData.reset(new PatientData(mDataService));
}

PatientService::~PatientService()
{
	mPatientData.reset();
	this->clearCache();
}

PatientDataPtr PatientService::getPatientData()
{
	return mPatientData;
}

DataServicePtr PatientService::getDataService()
{
	return mDataService;
}

//---------------------------------------------------------

//PatientService* patientService()
//{
//	return PatientService::getInstance();
//}

}
