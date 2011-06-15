/*
 * cxPatientService.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: christiana
 */

#include <cxPatientService.h>
#include "cxPatientData.h"

namespace cx
{

// --------------------------------------------------------
PatientService* PatientService::mInstance = NULL; ///< static member
// --------------------------------------------------------

void PatientService::initialize()
{
	PatientService::getInstance();
}

void PatientService::shutdown()
{
  delete mInstance;
  mInstance = NULL;
}

PatientService* PatientService::getInstance()
{
	if (!mInstance)
	{
		PatientService::setInstance(new PatientService());
	}
	return mInstance;
}

void PatientService::setInstance(PatientService* instance)
{
	if (mInstance)
	{
		delete mInstance;
	}
	mInstance = instance;
}

PatientService::PatientService()
{
	mPatientData.reset(new PatientData());
}

PatientService::~PatientService()
{

}

PatientDataPtr PatientService::getPatientData()
{
	return mPatientData;
}

//---------------------------------------------------------

PatientService* patientService()
{
	return PatientService::getInstance();
}

}
