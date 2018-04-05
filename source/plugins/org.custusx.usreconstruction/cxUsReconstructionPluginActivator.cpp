/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUsReconstructionPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxUsReconstructionGUIExtenderService.h"
#include "cxRegisteredService.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxUsReconstructionImplService.h"
#include "cxXmlOptionItem.h"
#include "cxUsReconstructionServiceProxy.h"
#include "cxProfile.h"
#include "cxLogger.h"
#include "cxFileManagerServiceProxy.h"


namespace cx
{

UsReconstructionPluginActivator::UsReconstructionPluginActivator()
{
}

UsReconstructionPluginActivator::~UsReconstructionPluginActivator()
{
}


void UsReconstructionPluginActivator::start(ctkPluginContext* context)
{
	PatientModelServicePtr patientModelService = PatientModelServicePtr(new PatientModelServiceProxy(context));
	ViewServicePtr viewService = ViewServicePtr(new ViewServiceProxy(context));
	FileManagerServicePtr filemanagerservice = FileManagerServicePtr(new FileManagerServiceProxy(context));

	XmlOptionFile xmlFile = profile()->getXmlSettings().descend("usReconstruction");
	UsReconstructionImplService *usReconstructionService = new UsReconstructionImplService(context, patientModelService, viewService, filemanagerservice, xmlFile);

	mUsReconstruction = RegisteredServicePtr(new RegisteredService(context, usReconstructionService, UsReconstructionService_iid));

	UsReconstructionServicePtr usReconstructionServiceProxy = UsReconstructionServicePtr(new UsReconstructionServiceProxy(context));

	UsReconstructionGUIExtenderService *guiService = new UsReconstructionGUIExtenderService(usReconstructionServiceProxy, patientModelService);
	mRegisteredGui = RegisteredServicePtr(new RegisteredService(context, guiService, GUIExtenderService_iid));

}

void UsReconstructionPluginActivator::stop(ctkPluginContext* context)
{
	mRegisteredGui.reset();
	mUsReconstruction.reset();
	Q_UNUSED(context);
}

} // namespace cx


