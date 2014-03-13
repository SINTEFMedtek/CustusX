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
#include "cxMessageManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxTypeConversions.h"
#include "cxPlaybackUSAcquisitionVideo.h"
#include "cxViewManager.h"
#include "cxLegacySingletons.h"

namespace cx
{

ServiceController::ServiceController()
{
	// load the ever-present video stream into the patient service
//	dataManager()->loadStream(videoService()->getActiveVideoSource());

	// connecting the video source and the tracking us probe.
//	connect(toolManager(), SIGNAL(configured()), this, SLOT(updateVideoConnections()));
//	connect(toolManager(), SIGNAL(initialized()), this, SLOT(updateVideoConnections()));
//	connect(toolManager(), SIGNAL(dominantToolChanged(QString)), this, SLOT(updateVideoConnections()));
////	connect(videoService()->getVideoConnection().get(), SIGNAL(connected(bool)), this, SLOT(updateVideoConnections()));
//	connect(videoService(), SIGNAL(activeVideoSourceChanged()), this, SLOT(updateVideoConnections()));

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

	toolManager()->setLoggingFolder(loggingPath);
	messageManager()->setLoggingFolder(loggingPath);
}

void ServiceController::clearPatientSlot()
{
	toolManager()->clear();
	viewManager()->clear();
}

void ServiceController::duringSavePatientSlot()
{
	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");

	toolManager()->addXml(managerNode);
	toolManager()->savePositionHistory();

	viewManager()->addXml(managerNode);
}

void ServiceController::duringLoadPatientSlot()
{
	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");

	QDomNode toolmanagerNode = managerNode.namedItem("toolManager");
	toolManager()->parseXml(toolmanagerNode);

	QDomNode viewmanagerNode = managerNode.namedItem("viewManager");
	viewManager()->parseXml(viewmanagerNode);
}


}
