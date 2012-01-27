/*
 * cxUsReconstructionPlugin.cpp
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#include <cxUsReconstructionPlugin.h>
#include "cxDataLocations.h"
#include "sscXmlOptionItem.h"
#include "sscReconstructManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

#include "sscReconstructionWidget.h"


namespace cx
{

UsReconstructionPlugin::UsReconstructionPlugin()
{
  ssc::XmlOptionFile xmlFile = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("usReconstruction");
  mReconstructer.reset(new ssc::ReconstructManager(xmlFile, DataLocations::getShaderPath()));

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
			new ssc::ReconstructionWidget(NULL, mReconstructer),
			"Algorithms"));

	return retval;
}

}
