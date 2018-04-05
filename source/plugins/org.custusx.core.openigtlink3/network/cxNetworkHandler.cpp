/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNetworkHandler.h"

#include <QTimer>

#include "igtlioLogic.h"
#include "igtlioImageDevice.h"
#include "igtlioTransformDevice.h"
#include "igtlioCommandDevice.h"
#include "igtlioStatusDevice.h"
#include "igtlioStringDevice.h"

#include "igtlioConnector.h"

#include "igtlioImageConverter.h"
#include "igtlioTransformConverter.h"
#include "igtlioCommandConverter.h"
#include "igtlioStatusConverter.h"
#include "igtlioStringConverter.h"

#include "cxLogger.h"

namespace cx
{

NetworkHandler::NetworkHandler(igtlio::LogicPointer logic) :
	mTimer(new QTimer(this)),
	mProbeDefinitionFromStringMessages(ProbeDefinitionFromStringMessagesPtr(new ProbeDefinitionFromStringMessages))
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

igtlio::SessionPointer NetworkHandler::requestConnectToServer(std::string serverHost, int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
	mSession = mLogic->ConnectToServer(serverHost, serverPort, sync, timeout_s);
	return mSession;
}

void NetworkHandler::disconnectFromServer()
{
	if (mSession->GetConnector() && mSession->GetConnector()->GetState()!=igtlio::Connector::STATE_OFF)
	{
		CX_LOG_DEBUG() << "NetworkHandler: Disconnecting from server" << mSession->GetConnector()->GetName();
		igtlio::ConnectorPointer connector = mSession->GetConnector();
		if(connector->GetState() != igtlio::Connector::STATE_WAIT_CONNECTION) //Don't try to stop connection while in STATE_WAIT_CONNECTION.
		{
			connector->Stop();//This takes to long if connect isn't finished (because mutex is blocking)
		}
		else
		{
			connector->SetServerStopFlag(true); //Just setting stop flag should also stop the server
		}
		mLogic->RemoveConnector(connector);
	}
	mProbeDefinitionFromStringMessages->reset();
}

void NetworkHandler::onDeviceReceived(vtkObject* caller_device, void* unknown, unsigned long event , void*)
{
	Q_UNUSED(unknown);
	Q_UNUSED(event);
	vtkSmartPointer<igtlio::Device> receivedDevice(reinterpret_cast<igtlio::Device*>(caller_device));

	igtlio::BaseConverter::HeaderData header = receivedDevice->GetHeader();
	std::string device_type = receivedDevice->GetDeviceType();

//	CX_LOG_DEBUG() << "Device is modified, device type: " << device_type << " on device: " << receivedDevice->GetDeviceName() << " equipmentId: " << header.equipmentId;

	// Currently the only id available is the Device name defined in PLUS xml. Looking like this: Probe_sToReference_s
	// Use this for all message types for now, instead of equipmentId.
	// Anser integration may send equipmentId, so this is checked for when we get a transform.
	QString deviceName(receivedDevice->GetDeviceName().c_str());

	if(device_type == igtlio::ImageConverter::GetIGTLTypeName())
	{
		igtlio::ImageDevicePointer imageDevice = igtlio::ImageDevice::SafeDownCast(receivedDevice);

		igtlio::ImageConverter::ContentData content = imageDevice->GetContent();

//		QString deviceName(header.deviceName.c_str());
//		QString deviceName(header.equipmentId.c_str());//Use equipmentId
		ImagePtr cximage = ImagePtr(new Image(deviceName, content.image));
		// get timestamp from igtl second-format:;
		double timestampMS = header.timestamp * 1000;
		cximage->setAcquisitionTime( QDateTime::fromMSecsSinceEpoch(qint64(timestampMS)));
		//this->decode_rMd(msg, retval);

		mProbeDefinitionFromStringMessages->setImage(cximage);

		if (mProbeDefinitionFromStringMessages->haveValidValues() && mProbeDefinitionFromStringMessages->haveChanged())
		{
//			QString deviceName(header.deviceName.c_str());
//			QString deviceName(header.equipmentId.c_str());//Use equipmentId instead?

			emit probedefinition(deviceName, mProbeDefinitionFromStringMessages->createProbeDefintion(deviceName));
		}


		emit image(cximage);
	}
	else if(device_type == igtlio::TransformConverter::GetIGTLTypeName())
	{
		igtlio::TransformDevicePointer transformDevice = igtlio::TransformDevice::SafeDownCast(receivedDevice);
		igtlio::TransformConverter::ContentData content = transformDevice->GetContent();

//		QString deviceName(content.deviceName.c_str());
//		QString deviceName(header.equipmentId.c_str());//Use equipmentId
		//QString streamIdTo(content.streamIdTo.c_str());
		//QString streamIdFrom(content.streamIdFrom.c_str());
		Transform3D cxtransform = Transform3D::fromVtkMatrix(content.transform);

//		CX_LOG_DEBUG() << "TRANSFORM: "	<< " equipmentId: " << header.equipmentId
//										<< " streamIdTo: " << content.streamIdTo
//										<< " streamIdFrom: " << content.streamIdFrom
//										<< " deviceName: " << deviceName
//										<< " transform: " << cxtransform;

		double timestamp = header.timestamp;
//		emit transform(deviceName, header.equipmentType, cxtransform, timestamp);
		//test: Set all messages as type TRACKED_US_PROBE for now
//		emit transform(deviceName, igtlio::BaseConverter::TRACKED_US_PROBE, cxtransform, timestamp);

		// Try to use equipmentId from OpenIGTLink meta data. If not presnet use deviceName.
		// Having equipmentId in OpenIGTLink meta data is something we would like to have a part of the OpenIGTLinkIO standard,
		// and added to the messages from PLUS.
		std::string openigtlinktransformid;
		bool gotTransformId = receivedDevice->GetMetaDataElement("equipmentId", openigtlinktransformid);

		if (gotTransformId)
			emit transform(qstring_cast(openigtlinktransformid), cxtransform, timestamp);
		else
			emit transform(deviceName, cxtransform, timestamp);
	}
	else if(device_type == igtlio::CommandConverter::GetIGTLTypeName())
	{
		CX_LOG_DEBUG() << "Received command message.";
		igtlio::CommandDevicePointer command = igtlio::CommandDevice::SafeDownCast(receivedDevice);

		igtlio::CommandConverter::ContentData content = command->GetContent();
		CX_LOG_DEBUG() << "COMMAND: "	<< " id: " << content.id
										<< " name: " << content.name
										<< " content: " << content.content;
		QString deviceName(content.name.c_str());
		QString xml(content.content.c_str());
		emit commandRespons(deviceName, xml);

	}
	else if(device_type == igtlio::StatusConverter::GetIGTLTypeName())
	{
		igtlio::StatusDevicePointer status = igtlio::StatusDevice::SafeDownCast(receivedDevice);

		igtlio::StatusConverter::ContentData content = status->GetContent();

		CX_LOG_DEBUG() << "STATUS: "	<< " code: " << content.code
										<< " subcode: " << content.subcode
										<< " errorname: " << content.errorname
										<< " statusstring: " << content.statusstring;

	}
	else if(device_type == igtlio::StringConverter::GetIGTLTypeName())
	{
		igtlio::StringDevicePointer string = igtlio::StringDevice::SafeDownCast(receivedDevice);

		igtlio::StringConverter::ContentData content = string->GetContent();

//		CX_LOG_DEBUG() << "STRING: "	<< " equipmentId: " << header.equipmentId
//										<< " encoding: " << content.encoding
//										<< " string: " << content.string_msg;

		QString message(content.string_msg.c_str());
		mProbeDefinitionFromStringMessages->parseStringMessage(header, message);
		emit string_message(message);
	}
	else
	{
		CX_LOG_WARNING() << "Found unhandled devicetype: " << device_type;
	}

}

void NetworkHandler::onConnectionEvent(vtkObject* caller, void* connector, unsigned long event , void*)
{
	Q_UNUSED(caller);
	Q_UNUSED(connector);
	if (event==igtlio::Logic::ConnectionAddedEvent)
	{
		emit connected();
	}
	if (event==igtlio::Logic::ConnectionAboutToBeRemovedEvent)
	{
		emit disconnected();
	}
}

void NetworkHandler::onDeviceAddedOrRemoved(vtkObject* caller, void* void_device, unsigned long event, void* callData)
{
	Q_UNUSED(caller);
	Q_UNUSED(callData);
	if (event==igtlio::Logic::NewDeviceEvent)
	{
		igtlio::DevicePointer device(reinterpret_cast<igtlio::Device*>(void_device));
		if(device)
		{
			CX_LOG_DEBUG() << " NetworkHandler is listening to " << device->GetDeviceName();
			qvtkReconnect(NULL, device, igtlio::Device::ReceiveEvent, this, SLOT(onDeviceReceived(vtkObject*, void*, unsigned long, void*)));
		}
	}
	if (event==igtlio::Logic::RemovedDeviceEvent)
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
			<< igtlio::Logic::ConnectionAddedEvent
			<< igtlio::Logic::ConnectionAboutToBeRemovedEvent
			)
	{
		qvtkReconnect(NULL, mLogic, eventId,
					  this, SLOT(onConnectionEvent(vtkObject*, void*, unsigned long, void*)));
	}
}

void NetworkHandler::connectToDeviceEvents()
{
	foreach(int eventId, QList<int>()
			<< igtlio::Logic::NewDeviceEvent
			<< igtlio::Logic::RemovedDeviceEvent
			)
	{
		qvtkReconnect(NULL, mLogic, eventId,
					this, SLOT(onDeviceAddedOrRemoved(vtkObject*, void*, unsigned long, void*)));
	}
}

} // namespace cx
