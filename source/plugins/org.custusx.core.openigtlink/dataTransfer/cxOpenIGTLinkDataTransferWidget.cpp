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
#include "cxOpenIGTLinkDataTransferWidget.h"

#include "cxOpenIGTLinkClient.h"
#include "cxOpenIGTLinkConnectionWidget.h"
#include "cxBoolProperty.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"

namespace cx {

OpenIGTLinkDataTransferWidget::OpenIGTLinkDataTransferWidget(ctkPluginContext *context, QWidget *parent) :
	BaseWidget(parent, "OpenIGTLinkDataTransferWidget", "OpenIGTLink Data Transfer")
{
	mOptions = profile()->getXmlSettings().descend(this->getConfigUid());

	mPatientModelService = PatientModelServiceProxy::create(context);
	mViewService = VisualizationServiceProxy::create(context);

	mOpenIGTLink.reset(new OpenIGTLinkClientThreadHandler(this->getConfigUid()));

//	mOpenIGTLinkThread.setObjectName("org.custusx.core.openigtlink.datatransfer");
//	mClient = new OpenIGTLinkClient;
//	mClient->moveToThread(&mOpenIGTLinkThread);
//	mOpenIGTLinkThread.start();

	connect(mOpenIGTLink->client(), &OpenIGTLinkClient::image, this, &OpenIGTLinkDataTransferWidget::onImageReceived);

	mConnectionWidget = new OpenIGTLinkConnectionWidget(mOpenIGTLink->client());


	mAcceptIncomingData = BoolProperty::initialize("acceptIncoming", "Accept Incoming",
												   "Accept incoming data and add to Patient Model",
												   true, mOptions.getElement());

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(mConnectionWidget);
	layout->addWidget(createDataWidget(mViewService, mPatientModelService, this, mAcceptIncomingData));
}

OpenIGTLinkDataTransferWidget::~OpenIGTLinkDataTransferWidget()
{
	mOpenIGTLink.reset(); //
}

QString OpenIGTLinkDataTransferWidget::getConfigUid() const
{
	return "org.custusx.core.openigtlink.datatransfer";
}

void OpenIGTLinkDataTransferWidget::onImageReceived(ImagePtr image)
{
	QString actionText = mAcceptIncomingData->getValue() ? "inserting" : "ignoring";
	CX_LOG_CHANNEL_INFO("org.custusx.core.openigtlink") << QString("Received image [%1] over IGTLink, %2")
														   .arg(image->getName())
														   .arg(actionText);

	if (mAcceptIncomingData->getValue())
	{
		mPatientModelService->insertData(image);
		mViewService->autoShowData(image);
	}
}

} // namespace cx
