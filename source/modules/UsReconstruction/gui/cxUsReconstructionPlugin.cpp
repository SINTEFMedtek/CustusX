/*
 * cxUsReconstructionPlugin.cpp
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

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
  mReconstructer->init();

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
