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
#include "cxOpenIGTLinkDataTransfer.h"

#include "cxOpenIGTLinkClient.h"
#include "cxBoolProperty.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxSelectDataStringProperty.h"
#include "cxStringProperty.h"
#include "cxMesh.h"
#include "cxIGTLinkConversionPolyData.h"

namespace cx {

OpenIGTLinkDataTransfer::OpenIGTLinkDataTransfer(ctkPluginContext *context, QObject* parent) :
	QObject(parent)
{
	mOptions = profile()->getXmlSettings().descend(this->getConfigUid());

	mPatientModelService = PatientModelServiceProxy::create(context);
	mViewService = VisualizationServiceProxy::create(context);

	mOpenIGTLink.reset(new OpenIGTLinkClientThreadHandler(this->getConfigUid()));

	connect(mOpenIGTLink->client(), &OpenIGTLinkClient::image, this, &OpenIGTLinkDataTransfer::onImageReceived);
	connect(mOpenIGTLink->client(), &OpenIGTLinkClient::mesh, this, &OpenIGTLinkDataTransfer::onMeshReceived);

	mDataToSend = StringPropertySelectData::New(mPatientModelService);

	mAcceptIncomingData = BoolProperty::initialize("acceptIncoming", "Accept Incoming",
												   "Accept incoming data and add to Patient Model",
												   true, mOptions.getElement());
}

OpenIGTLinkDataTransfer::~OpenIGTLinkDataTransfer()
{
	mOpenIGTLink.reset(); //
}

OpenIGTLinkClient* OpenIGTLinkDataTransfer::getOpenIGTLink()
{
	return mOpenIGTLink->client();
}

QString OpenIGTLinkDataTransfer::getConfigUid() const
{
	return "org.custusx.core.openigtlink.datatransfer";
}

void OpenIGTLinkDataTransfer::onImageReceived(ImagePtr image)
{
	this->onDataReceived(image);
}

void OpenIGTLinkDataTransfer::onMeshReceived(MeshPtr mesh)
{
	this->onDataReceived(mesh);
}

void OpenIGTLinkDataTransfer::onDataReceived(DataPtr data)
{
	QString actionText = mAcceptIncomingData->getValue() ? "inserting" : "ignoring";
	QString nameText = data ? data->getName() : "NULL";
	CX_LOG_CHANNEL_INFO(this->getConfigUid()) << QString("Received image [%1] over IGTLink, %2")
														   .arg(nameText)
														   .arg(actionText);

	if (mAcceptIncomingData->getValue())
	{
		mPatientModelService->insertData(data);
		mViewService->autoShowData(data);
	}
}


void OpenIGTLinkDataTransfer::onSend()
{
	DataPtr data = mDataToSend->getData();
	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	if (image)
	{
		mOpenIGTLink->client()->sendMessage(image);
		return;
	}
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(data);
	if (mesh)
	{
//		// test begin
//		IGTLinkConversionPolyData polyConverter;
//		igtl::PolyDataMessage::Pointer msg = polyConverter.encode(mesh, pcsRAS);
//		CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "Debbuging mesh: " << data->getName();

//		MeshPtr retval = polyConverter.decode(msg, pcsRAS);
//		mPatientModelService->insertData(retval);
//		return;
//		// test end

		mOpenIGTLink->client()->sendMessage(mesh);
		return;
	}

	QString name = data ? data->getName() : "NULL";
	CX_LOG_CHANNEL_INFO(this->getConfigUid()) << QString("Failed to send data %1 over igtl: Unsupported type")
														   .arg(name);
}

} // namespace cx
