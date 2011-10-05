/*
 * cxAcquisitionPlugin.cpp
 *
 *  Created on: Jun 15, 2011
 *      Author: christiana
 */

#include <cxAcquisitionPlugin.h>

#include <vector>
#include <QDomNode>
#include <QDateTime>
#include <QStringList>
#include "sscTime.h"
#include "cxAcquisitionData.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

#include "cxUSAcqusitionWidget.h"
#include "cxTrackedCenterlineWidget.h"

namespace cx
{

AcquisitionPlugin::AcquisitionPlugin(ssc::ReconstructManagerPtr reconstructer)
{
	mAcquisitionData.reset(new AcquisitionData(reconstructer));

	connect(patientService()->getPatientData().get(), SIGNAL(isSaving()), this, SLOT(duringSavePatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(isLoading()), this, SLOT(duringLoadPatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(cleared()), this, SLOT(clearSlot()));
}

AcquisitionPlugin::~AcquisitionPlugin()
{

}

std::vector<PluginBase::PluginWidget> AcquisitionPlugin::createWidgets() const
{
	std::vector<PluginWidget> retval;

	retval.push_back(PluginBase::PluginWidget(new USAcqusitionWidget(mAcquisitionData, NULL), "Utility"));

	retval.push_back(PluginBase::PluginWidget(new TrackedCenterlineWidget(mAcquisitionData, NULL), "Utility"));

	return retval;

}

void AcquisitionPlugin::addXml(QDomNode& parentNode)
{
	mAcquisitionData->addXml(parentNode);
}

void AcquisitionPlugin::parseXml(QDomNode& dataNode)
{
	mAcquisitionData->parseXml(dataNode);
}

void AcquisitionPlugin::clearSlot()
{
	// clear data?
}

void AcquisitionPlugin::duringSavePatientSlot()
{
	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");
	this->addXml(managerNode);
}

void AcquisitionPlugin::duringLoadPatientSlot()
{
	QDomElement stateManagerNode =
					patientService()->getPatientData()->getCurrentWorkingElement("managers/stateManager");
	this->parseXml(stateManagerNode);
}

}
