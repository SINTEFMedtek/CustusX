/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDicomPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxDicomGUIExtenderService.h"

#include "cxRegisteredService.h"


namespace cx
{

DicomPluginActivator::DicomPluginActivator()
{
//	std::cout << "Created DicomPluginActivator" << std::endl;
}

DicomPluginActivator::~DicomPluginActivator()
{
}

void DicomPluginActivator::start(ctkPluginContext* context)
{
//	std::cout << "Starting DicomPluginActivator" << std::endl;
	mRegistration = RegisteredService::create<DicomGUIExtenderService>(context, GUIExtenderService_iid);
}

void DicomPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	//	std::cout << "Stopped DicomPluginActivator" << std::endl;
	Q_UNUSED(context);
}

} // namespace cx


