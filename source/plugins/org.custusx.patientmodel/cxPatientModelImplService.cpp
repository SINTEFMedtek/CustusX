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

#include "cxPatientModelImplService.h"

#include "cxData.h"
#include "cxReporter.h"
#include "cxLogicManager.h"
#include "cxDataManager.h"
#include "cxPatientData.h"
#include "cxPatientService.h"

namespace cx
{


PatientModelImplService::PatientModelImplService()
{
}

PatientModelImplService::~PatientModelImplService()
{
}

void PatientModelImplService::insertData(DataPtr data)
{
	LogicManager* lm = LogicManager::getInstance();
	lm->getDataService()->loadData(data);
	QString outputBasePath = lm->getPatientService()->getPatientData()->getActivePatientFolder();
	lm->getDataService()->saveData(data, outputBasePath);
}


} /* namespace cx */
