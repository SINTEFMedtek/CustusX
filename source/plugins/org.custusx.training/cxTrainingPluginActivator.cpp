/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrainingPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxTrainingGUIExtenderService.h"
#include "cxRegisteredService.h"
//#include "cxTrainingEngine.h"

namespace cx
{

TrainingPluginActivator::TrainingPluginActivator()
{
//	std::cout << "Created TrainingPluginActivator" << std::endl;
}

TrainingPluginActivator::~TrainingPluginActivator()
{}

void TrainingPluginActivator::start(ctkPluginContext* context)
{
//	mEngine.reset(new TrainingEngine);

	TrainingGUIExtenderService* guiExtender = new TrainingGUIExtenderService(context);
	mGUIExtender = RegisteredService::create<TrainingGUIExtenderService>(context, guiExtender, GUIExtenderService_iid);
}

void TrainingPluginActivator::stop(ctkPluginContext* context)
{
	mGUIExtender.reset();
//	mEngine.reset();
    Q_UNUSED(context);
}

} // namespace cx
