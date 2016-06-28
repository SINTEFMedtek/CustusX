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

#include "cxNetworkHandler.h"

#include <QTimer>

#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOImageDevice.h"
#include "vtkIGTLIOTransformDevice.h"
#include "vtkIGTLIOCommandDevice.h"
#include "vtkIGTLIOStatusDevice.h"

#include "vtkIGTLIOConnector.h"

#include "igtlImageConverter.h"
#include "igtlTransformConverter.h"
#include "igtlCommandConverter.h"
#include "igtlStatusConverter.h"

#include "cxLogger.h"

namespace cx
{

NetworkHandler::NetworkHandler(vtkIGTLIOLogicPointer logic) :
	mTimer(new QTimer(this))
{
	qRegisterMetaType<Transform3D>("Transform3D");
	qRegisterMetaType<ImagePtr>("ImagePtr");

	mLogic = logic;

	this->connectToConnectionEvents();
	this->connectToDeviceEvents();

	connect(mTimer, SIGNAL(timeout()), this, SLOT(periodicProcess()));
	mTimer->start(5);
}

NetworkHandler::~NetworkHandler()
{
	mTimer->stop();
}

vtkIGTLIOSessionPointer NetworkHandler::requestConnectToServer(std::string serverHost, int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
	mSession = mLogic->ConnectToServer(serverHost, serverPort, sync, timeout_s);
	return mSession;
}

void NetworkHandler::onDeviceModified(vtkObject* caller_device, void* unknown, unsigned long event , void*)
{
	vtkSmartPointer<vtkIGTLIODevice> receivedDevice(reinterpret_cast<vtkIGTLIODevice*>(caller_device));

	igtl::BaseConverter::HeaderData header = receivedDevice->GetHeader();
	std::string device_type = receivedDevice->GetDeviceType();

	CX_LOG_DEBUG() << "Incoming " << device_type << " on device: " << receivedDevice->GetDeviceName();

	if(device_type == igtl::ImageConverter::GetIGTLTypeName())
	{
		vtkIGTLIOImageDevicePointer imageDevice = vtkIGTLIOImageDevice::SafeDownCast(receivedDevice);

		igtl::ImageConverter::ContentData content = imageDevice->GetContent();

		QString deviceName(header.deviceName.c_str());
		ImagePtr cximage = ImagePtr(new Image(deviceName, content.image));
		// get timestamp from igtl second-format:;
		double timestampMS = header.timestamp * 1000;
		cximage->setAcquisitionTime( QDateTime::fromMSecsSinceEpoch(timestampMS));
		//this->decode_rMd(msg, retval);

		emit image(cximage);
	}
	else if(device_type == igtl::TransformConverter::GetIGTLTypeName())
	{
		vtkIGTLIOTransformDevicePointer transformDevice = vtkIGTLIOTransformDevice::SafeDownCast(receivedDevice);
		igtl::TransformConverter::ContentData content = transformDevice->GetContent();

		QString deviceName(content.deviceName.c_str());
		Transform3D cxtransform = Transform3D::fromVtkMatrix(content.transform);
		double timestamp = header.timestamp;
		emit transform(deviceName, cxtransform, timestamp);
	}
	else if(device_type == igtl::CommandConverter::GetIGTLTypeName())
	{
		CX_LOG_DEBUG() << "Received command message.";
		vtkIGTLIOCommandDevicePointer command = vtkIGTLIOCommandDevice::SafeDownCast(receivedDevice);

		igtl::CommandConverter::ContentData content = command->GetContent();
		CX_LOG_DEBUG() << "COMMAND: "	<< " id: " << content.id
										<< " name: " << content.name
										<< " content: " << content.content;
		//TODO
		QString deviceName(content.name.c_str());
		QString xml(content.content.c_str());
		emit commandRespons(deviceName, xml);

	}
	else if(device_type == igtl::StatusConverter::GetIGTLTypeName())
	{
		vtkIGTLIOStatusDevicePointer status = vtkIGTLIOStatusDevice::SafeDownCast(receivedDevice);

		igtl::StatusConverter::ContentData content = status->GetContent();
		//TODO

	}
	else
	{
		CX_LOG_WARNING() << "Found unhandled devicetype: " << device_type;
	}

}

void NetworkHandler::onConnectionEvent(vtkObject* caller, void* connector, unsigned long event , void*)
{
	if (event==vtkIGTLIOLogic::ConnectionAddedEvent)
	{
		emit connected();
	}
	if (event==vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
	{
		emit disconnected();
	}
}

void NetworkHandler::onDeviceAddedOrRemoved(vtkObject* caller, void* void_device, unsigned long event, void* callData)
{
	if (event==vtkIGTLIOLogic::NewDeviceEvent)
	{
		vtkIGTLIODevicePointer device(reinterpret_cast<vtkIGTLIODevice*>(void_device));
		if(device)
		{
			CX_LOG_DEBUG() << " NetworkHandler is listening to " << device->GetDeviceName();
			qvtkReconnect(NULL, device, vtkIGTLIODevice::ModifiedEvent, this, SLOT(onDeviceModified(vtkObject*, void*, unsigned long, void*)));
		}
	}
	if (event==vtkIGTLIOLogic::RemovedDeviceEvent)
	{
		CX_LOG_WARNING() << "TODO: on remove device event, not implemented";
	}
}

void NetworkHandler::periodicProcess()
{
	mLogic->PeriodicProcess();
}

void NetworkHandler::connectToConnectionEvents()
{
	foreach(int eventId, QList<int>()
			<< vtkIGTLIOLogic::ConnectionAddedEvent
			<< vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent
			)
	{
		qvtkReconnect(NULL, mLogic, eventId,
					  this, SLOT(onConnectionEvent(vtkObject*, void*, unsigned long, void*)));
	}
}

void NetworkHandler::connectToDeviceEvents()
{
	foreach(int eventId, QList<int>()
			<< vtkIGTLIOLogic::NewDeviceEvent
			<< vtkIGTLIOLogic::RemovedDeviceEvent
			)
	{
		qvtkReconnect(NULL, mLogic, eventId,
					this, SLOT(onDeviceAddedOrRemoved(vtkObject*, void*, unsigned long, void*)));
	}
}

} // namespace cx
