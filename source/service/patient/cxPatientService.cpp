// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
	mThresholdPreview.reset(new ThresholdPreview());
}

PatientService::~PatientService()
{

}

PatientDataPtr PatientService::getPatientData()
{
	return mPatientData;
}

ThresholdPreviewPtr PatientService::getThresholdPreview()
{
	return mThresholdPreview;
}

//---------------------------------------------------------

PatientService* patientService()
{
	return PatientService::getInstance();
}

}
