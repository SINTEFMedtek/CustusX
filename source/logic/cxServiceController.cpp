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
#include <cxServiceController.h>

#include <QDir>

#include "cxTrackingService.h"
#include "cxVideoService.h"
#include "cxReporter.h"
#include "cxTypeConversions.h"
//#include "cxPlaybackUSAcquisitionVideo.h"
#include "cxViewManager.h"
#include "cxLegacySingletons.h"
#include "cxPatientModelService.h"

namespace cx
{

ServiceController::ServiceController()
{
	connect(patientService().get(), SIGNAL(isSaving()), this, SLOT(duringSavePatientSlot()));
	connect(patientService().get(), SIGNAL(isLoading()), this, SLOT(duringLoadPatientSlot()));
	connect(patientService().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
	connect(patientService().get(), SIGNAL(cleared()), this, SLOT(clearPatientSlot()));
}

ServiceController::~ServiceController()
{
}

void ServiceController::patientChangedSlot()
{
	QString patientFolder = patientService()->getActivePatientFolder();
	if (patientFolder.isEmpty())
		return;

	QString loggingPath = patientFolder + "/Logs/";
	QDir loggingDir(loggingPath);
	if (!loggingDir.exists())
	{
		loggingDir.mkpath(loggingPath);
	}
//	videoService()->getUSAcquisitionVideoPlayback()->setRoot(patientService()->getActivePatientFolder() + "/US_Acq/");

	trackingService()->setLoggingFolder(loggingPath);
	reporter()->setLoggingFolder(loggingPath);
}

void ServiceController::clearPatientSlot()
{
	trackingService()->clear();
	viewManager()->clear();
}

void ServiceController::duringSavePatientSlot()
{
	QDomElement managerNode = patientService()->getCurrentWorkingElement("managers");

	trackingService()->addXml(managerNode);
	trackingService()->savePositionHistory();

	viewManager()->addXml(managerNode);
}

void ServiceController::duringLoadPatientSlot()
{
	QDomElement managerNode = patientService()->getCurrentWorkingElement("managers");

	QDomNode toolmanagerNode = managerNode.namedItem("toolManager");
	trackingService()->parseXml(toolmanagerNode);

	QDomNode viewmanagerNode = managerNode.namedItem("viewManager");
	viewManager()->parseXml(viewmanagerNode);
}


}
