/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxOpenIGTLinkGuiExtenderService.h"
#include "qIGTLIOClientWidget.h"
#include "qIGTLIOLogicController.h"
#include "cxPlusConnectWidget.h"
#include "cxVisServices.h"

namespace cx
{
OpenIGTLink3GuiExtenderService::OpenIGTLink3GuiExtenderService(ctkPluginContext *context, igtlioLogicPointer logic)
{
	mContext = context;
	mLogic = logic;

}

OpenIGTLink3GuiExtenderService::~OpenIGTLink3GuiExtenderService()
{
}

std::vector<GUIExtenderService::CategorizedWidget> OpenIGTLink3GuiExtenderService::createWidgets() const
{
	qIGTLIOLogicController* logicController = new qIGTLIOLogicController();
	logicController->setLogic(mLogic);

	qIGTLIOClientWidget* widget = new qIGTLIOClientWidget();
	widget->setWindowTitle("OpenIGTLink3");
	widget->setObjectName("Object_OpenIGTLink_3");
	widget->setLogic(mLogic);

	std::vector<CategorizedWidget> retval;
	retval.push_back(GUIExtenderService::CategorizedWidget( widget, "OpenIGTLink"));


	VisServicesPtr services = VisServices::create(mContext);

	PlusConnectWidget* plusconnectWidget = new PlusConnectWidget(services, NULL);
	retval.push_back(GUIExtenderService::CategorizedWidget(plusconnectWidget, "OpenIGTLink"));

	return retval;
}
}//namespace cx
