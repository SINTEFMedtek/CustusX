/*
 * cxUsReconstructionPlugin.cpp
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#include <cxUsReconstructionPlugin.h>
#include "cxDataLocations.h"
#include "cxXmlOptionItem.h"
#include "cxReconstructManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

#include "cxReconstructionWidget.h"


namespace cx
{

UsReconstructionPlugin::UsReconstructionPlugin()
{
  XmlOptionFile xmlFile = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("usReconstruction");
  mReconstructer.reset(new ReconstructManager(xmlFile, DataLocations::getShaderPath()));

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

//  mReconstructionWidget->selectData(patientService()->getPatientData()->getActivePatientFolder() + "/US_Acq/");
//  mReconstructionWidget->reconstructer()->setOutputBasePath(patientService()->getPatientData()->getActivePatientFolder());
//  mReconstructionWidget->reconstructer()->setOutputRelativePath("Images");

//  this->updateWindowTitle();
}

std::vector<PluginBase::PluginWidget> UsReconstructionPlugin::createWidgets() const
{
	std::vector<PluginWidget> retval;

	retval.push_back(PluginBase::PluginWidget(
			new ReconstructionWidget(NULL, mReconstructer),
			"Algorithms"));

	return retval;
}

}
