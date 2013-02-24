/*
 * cxServiceController.cpp
 *
 *  \date Jun 1, 2011
 *      \author christiana
 */

#include <cxServiceController.h>

#include <QDir>

#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxVideoService.h"
#include "sscMessageManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "sscTypeConversions.h"
#include "cxPlaybackUSAcquisitionVideo.h"

namespace cx
{

ServiceController::ServiceController()
{
	// load the ever-present video stream into the patient service
	ssc::dataManager()->loadStream(videoService()->getActiveVideoSource());

	// connecting the video source and the tracking us probe.
	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateVideoConnections()));
	connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(updateVideoConnections()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(QString)), this, SLOT(updateVideoConnections()));
//	connect(videoService()->getVideoConnection().get(), SIGNAL(connected(bool)), this, SLOT(updateVideoConnections()));
	connect(videoService(), SIGNAL(activeVideoSourceChanged()), this, SLOT(updateVideoConnections()));

	connect(patientService()->getPatientData().get(), SIGNAL(isSaving()), this, SLOT(duringSavePatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(isLoading()), this, SLOT(duringLoadPatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(cleared()), this, SLOT(clearPatientSlot()));
}

ServiceController::~ServiceController()
{
}

void ServiceController::patientChangedSlot()
{
	QString patientFolder = patientService()->getPatientData()->getActivePatientFolder();

	QString loggingPath = patientFolder + "/Logs/";
	QDir loggingDir(loggingPath);
	if (!loggingDir.exists())
	{
		loggingDir.mkpath(loggingPath);
	}
	videoService()->getUSAcquisitionVideoPlayback()->setRoot(patientService()->getPatientData()->getActivePatientFolder() + "/US_Acq/");

	ToolManager::getInstance()->setLoggingFolder(loggingPath);
	ssc::messageManager()->setLoggingFolder(loggingPath);
}

void ServiceController::clearPatientSlot()
{
	ssc::toolManager()->clear();
}

void ServiceController::duringSavePatientSlot()
{
	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");

	ssc::toolManager()->addXml(managerNode);
	ssc::toolManager()->savePositionHistory();
}

void ServiceController::duringLoadPatientSlot()
{
	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");

	QDomNode toolmanagerNode = managerNode.namedItem("toolManager");
	ssc::toolManager()->parseXml(toolmanagerNode);
}

/**Connect a probe from Tracking Service to a video source in Video Service.
 *
 */
void ServiceController::updateVideoConnections()
{
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();

	this->connectVideoToProbe(tool);
}

/**insert the rt source into the (first) probe tool
 * in the tool manager.
 *
 * Apply time calibration to the source.
 *
 */
void ServiceController::connectVideoToProbe(ssc::ToolPtr probe)
{
	ssc::VideoSourcePtr source = videoService()->getActiveVideoSource();
	if (!source)
	{
		ssc::messageManager()->sendError("no rt source.");
		return;
	}

	// find probe in tool manager
	// set source in cxTool
	// insert timecalibration using config
	if (!source->isConnected())
		return;

	if (!probe)
		return;

	if (probe)
	{
		ssc::ProbePtr probeInterface = probe->getProbe();
		if (!probeInterface)
			return;
		probeInterface->setRTSource(source);
	}
}

}
