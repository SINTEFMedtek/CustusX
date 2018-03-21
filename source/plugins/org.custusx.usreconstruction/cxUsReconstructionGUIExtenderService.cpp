/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxUsReconstructionGUIExtenderService.h"

#include "cxPatientModelService.h"

#include "cxReconstructionWidget.h"
#include "cxPatientModelService.h"
#include "cxUsReconstructionService.h"


namespace cx
{

UsReconstructionGUIExtenderService::UsReconstructionGUIExtenderService(UsReconstructionServicePtr usReconstructionService, PatientModelServicePtr patientModelService) :
	mReconstructer(usReconstructionService)
{
}

UsReconstructionGUIExtenderService::~UsReconstructionGUIExtenderService()
{
}

std::vector<GUIExtenderService::CategorizedWidget> UsReconstructionGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new ReconstructionWidget(NULL, mReconstructer),
			"Algorithms"));

	return retval;
}

}
