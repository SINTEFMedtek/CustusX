/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOpenIGTLinkPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxOpenIGTLinkStreamerService.h"
#include "cxOpenIGTLinkTrackingSystemService.h"
#include "cxRegisteredService.h"

#include "cxOpenIGTLinkGuiExtenderService.h"
#include "cxTrackingServiceProxy.h"

#include "igtlioLogic.h"

namespace cx
{

OpenIGTLinkPluginActivator::OpenIGTLinkPluginActivator()
{
}

OpenIGTLinkPluginActivator::~OpenIGTLinkPluginActivator()
{
}

void OpenIGTLinkPluginActivator::start(ctkPluginContext* context)
{

	TrackingServicePtr trackingService = TrackingServiceProxy::create(context);

	igtlio::LogicPointer logic = igtlio::LogicPointer::New();
	mNetworkHandler.reset(new NetworkHandler(logic));
	OpenIGTLink3GuiExtenderService* gui = new OpenIGTLink3GuiExtenderService(context, logic);

	OpenIGTLinkTrackingSystemService* tracking = new OpenIGTLinkTrackingSystemService(mNetworkHandler);
	OpenIGTLinkStreamerService *streamer = new OpenIGTLinkStreamerService(mNetworkHandler, trackingService);

	mRegistrationGui = RegisteredService::create<OpenIGTLink3GuiExtenderService>(context, gui, GUIExtenderService_iid);
	mRegistrationTracking = RegisteredService::create<OpenIGTLinkTrackingSystemService>(context, tracking, TrackingSystemService_iid);
	mRegistrationStreaming = RegisteredService::create<OpenIGTLinkStreamerService>(context, streamer, StreamerService_iid);
}

void OpenIGTLinkPluginActivator::stop(ctkPluginContext* context)
{
	Q_UNUSED(context);
	mRegistrationGui.reset();
	mRegistrationStreaming.reset();
	mRegistrationTracking.reset();
}

} // namespace cx



