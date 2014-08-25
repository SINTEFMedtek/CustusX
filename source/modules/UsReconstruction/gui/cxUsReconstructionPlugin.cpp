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
#include <cxUsReconstructionPlugin.h>
#include "cxDataLocations.h"
#include "cxXmlOptionItem.h"
#include "cxReconstructionManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

#include "cxReconstructionWidget.h"


namespace cx
{

UsReconstructionPlugin::UsReconstructionPlugin()
{
  XmlOptionFile xmlFile = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("usReconstruction");
  mReconstructer.reset(new ReconstructionManager(xmlFile, DataLocations::getShaderPath()));
  //mReconstructer->init();

  connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

}

UsReconstructionPlugin::~UsReconstructionPlugin()
{
}

void UsReconstructionPlugin::patientChangedSlot()
{
    mReconstructer->selectData(patientService()->getPatientData()->getActivePatientFolder() + "/US_Acq/");
    mReconstructer->setOutputBasePath(patientService()->getPatientData()->getActivePatientFolder());
    mReconstructer->setOutputRelativePath("Images");

}

std::vector<GUIExtenderService::CategorizedWidget> UsReconstructionPlugin::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new ReconstructionWidget(NULL, mReconstructer),
			"Algorithms"));

	return retval;
}

}
