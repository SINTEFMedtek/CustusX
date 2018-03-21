/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxAcquisitionPlugin.h"

#include <vector>
#include <QDomNode>
#include <QDateTime>
#include <QStringList>
#include <ctkPluginContext.h>
#include "cxTime.h"
#include "cxAcquisitionData.h"

#include "cxUSAcqusitionWidget.h"

#include "cxPatientModelService.h"
#include "cxUsReconstructionServiceProxy.h"
#include "cxAcquisitionServiceProxy.h"
#include "cxVisServices.h"

namespace cx
{

AcquisitionPlugin::AcquisitionPlugin(ctkPluginContext *context) :
	mUsReconstructionService(new UsReconstructionServiceProxy(context)),
	mAcquisitionService(new AcquisitionServiceProxy(context))
{
	mServices = VisServices::create(context);
}

AcquisitionPlugin::~AcquisitionPlugin()
{

}

std::vector<GUIExtenderService::CategorizedWidget> AcquisitionPlugin::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(new USAcqusitionWidget(mAcquisitionService, mServices, mUsReconstructionService, NULL), "Utility"));

	return retval;

}

}
