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
#include "igtlioStatusDevice.h"
#include "igtlioStringDevice.h"

#include "igtlioConnector.h"

#include "igtlioImageConverter.h"
#include "igtlioTransformConverter.h"
#include "igtlioCommandConverter.h"
#include "igtlioStatusConverter.h"
#include "igtlioStringConverter.h"
#include "igtlioUsSectorDefinitions.h"

#include "cxLogger.h"

namespace cx
{

NetworkHandler::NetworkHandler(igtlioLogicPointer logic) :
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

igtlioSessionPointer NetworkHandler::requestConnectToServer(std::string serverHost, int serverPort, IGTLIO_SYNCHRONIZATION_TYPE sync, double timeout_s)
{
	mSession = mLogic->ConnectToServer(serverHost, serverPort, sync, timeout_s);
	return mSession;
}

void NetworkHandler::disconnectFromServer()
{
	if (mSession->GetConnector() && mSession->GetConnector()->GetState()!=igtlioConnector::STATE_OFF)
	{
		CX_LOG_DEBUG() << "NetworkHandler: Disconnecting from server" << mSession->GetConnector()->GetName();
		igtlioConnectorPointer connector = mSession->GetConnector();
		connector->Stop();
		mLogic->RemoveConnector(connector);
	}
	mProbeDefinitionFromStringMessages->reset();
}

void NetworkHandler::onDeviceReceived(vtkObject* caller_device, void* unknown, unsigned long event , void*)
{
	Q_UNUSED(unknown);
	Q_UNUSED(event);
	vtkSmartPointer<igtlioDevice> receivedDevice(reinterpret_cast<igtlioDevice*>(caller_device));

	igtlioBaseConverter::HeaderData header = receivedDevice->GetHeader();
	std::string device_type = receivedDevice->GetDeviceType();

//	CX_LOG_DEBUG() << "Device is modified, device type: " << device_type << " on device: " << receivedDevice->GetDeviceName() << " equipmentId: " << header.equipmentId;

	// Currently the only id available is the Device name defined in PLUS xml. Looking like this: Probe_sToReference_s
	// Use this for all message types for now, instead of equipmentId.
	// Anser integration may send equipmentId, so this is checked for when we get a transform.
	QString deviceName(receivedDevice->GetDeviceName().c_str());

	if(device_type == igtlioImageConverter::GetIGTLTypeName())
	{
		igtlioImageDevicePointer imageDevice = igtlioImageDevice::SafeDownCast(receivedDevice);

		igtlioImageConverter::ContentData content = imageDevice->GetContent();

//		QString deviceName(header.deviceName.c_str());
//		QString deviceName(header.equipmentId.c_str());//Use equipmentId
		ImagePtr cximage = ImagePtr(new Image(deviceName, content.image));
		// get timestamp from igtl second-format:;
		double timestampMS = header.timestamp * 1000;
		cximage->setAcquisitionTime( QDateTime::fromMSecsSinceEpoch(qint64(timestampMS)));
		//this->decode_rMd(msg, retval);


		//Use the igtlio meta data from the image message
		std::string metaLabel;
		std::string metaDataValue;
		QStringList igtlioLabels;

		igtlioLabels << IGTLIO_KEY_PROBE_TYPE;
		igtlioLabels << IGTLIO_KEY_ORIGIN;
		igtlioLabels << IGTLIO_KEY_ANGLES;
		igtlioLabels << IGTLIO_KEY_BOUNDING_BOX;
		igtlioLabels << IGTLIO_KEY_DEPTHS;
		igtlioLabels << IGTLIO_KEY_LINEAR_WIDTH;
		igtlioLabels << IGTLIO_KEY_SPACING_X;
		igtlioLabels << IGTLIO_KEY_SPACING_Y;
		//TODO: Use deciveNameLong when this is defined in IGTLIO and sent with PLUS


		for (int i = 0; i < igtlioLabels.size(); ++i)
		{
			metaLabel = igtlioLabels[i].toStdString();
			bool gotMetaData = receivedDevice->GetMetaDataElement(metaLabel, metaDataValue);
			if(!gotMetaData)
				CX_LOG_WARNING() << "Cannot get needed igtlio meta information: " << metaLabel;
			else
				mProbeDefinitionFromStringMessages->parseValue(metaLabel.c_str(), metaDataValue.c_str());
		}


		mProbeDefinitionFromStringMessages->setImage(cximage);

		if (mProbeDefinitionFromStringMessages->haveValidValues() && mProbeDefinitionFromStringMessages->haveChanged())
		{
			//TODO: Use deciveNameLong
			emit probedefinition(deviceName, mProbeDefinitionFromStringMessages->createProbeDefintion(deviceName));
		}

		emit image(cximage);

		// CX-366: Currenly we don't use the transform from the image message, because there is no specification of what this transform should be.
		// Only the transforms from the transform messages are used.
//		double timestamp = header.timestamp;
//		Transform3D cxtransform = Transform3D::fromVtkMatrix(content.transform);
//		emit transform(deviceName, cxtransform, timestamp);
	}
	else if(device_type == igtlioTransformConverter::GetIGTLTypeName())
	{
		igtlioTransformDevicePointer transformDevice = igtlioTransformDevice::SafeDownCast(receivedDevice);
		igtlioTransformConverter::ContentData content = transformDevice->GetContent();

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
//		emit transform(deviceName, igtlioBaseConverter::TRACKED_US_PROBE, cxtransform, timestamp);

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
	else if(device_type == igtlioStatusConverter::GetIGTLTypeName())
	{
		igtlioStatusDevicePointer status = igtlioStatusDevice::SafeDownCast(receivedDevice);

		igtlioStatusConverter::ContentData content = status->GetContent();

		CX_LOG_DEBUG() << "STATUS: "	<< " code: " << content.code
										<< " subcode: " << content.subcode
										<< " errorname: " << content.errorname
										<< " statusstring: " << content.statusstring;

	}
	else if(device_type == igtlioStringConverter::GetIGTLTypeName())
	{
		igtlioStringDevicePointer string = igtlioStringDevice::SafeDownCast(receivedDevice);

		igtlioStringConverter::ContentData content = string->GetContent();

//		CX_LOG_DEBUG() << "STRING: "	<< " equipmentId: " << header.equipmentId
//										<< " encoding: " << content.encoding
//										<< " string: " << content.string_msg;

		QString message(content.string_msg.c_str());
//		mProbeDefinitionFromStringMessages->parseStringMessage(header, message);//Turning this off because we want to use meta info instead
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
	if (event==igtlioLogic::ConnectionAddedEvent)
	{
		emit connected();
	}
	if (event==igtlioLogic::ConnectionAboutToBeRemovedEvent)
	{
		emit disconnected();
	}
}

void NetworkHandler::onDeviceAddedOrRemoved(vtkObject* caller, void* void_device, unsigned long event, void* callData)
{
	Q_UNUSED(caller);
	Q_UNUSED(callData);
	if (event==igtlioLogic::NewDeviceEvent)
	{
		igtlioDevicePointer device(reinterpret_cast<igtlioDevice*>(void_device));
		if(device)
		{
			CX_LOG_DEBUG() << " NetworkHandler is listening to " << device->GetDeviceName();
			qvtkReconnect(NULL, device, igtlioDevice::ReceiveEvent, this, SLOT(onDeviceReceived(vtkObject*, void*, unsigned long, void*)));
		}
	}
	if (event==igtlioLogic::RemovedDeviceEvent)
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
			<< igtlioLogic::ConnectionAddedEvent
			<< igtlioLogic::ConnectionAboutToBeRemovedEvent
			)
	{
		qvtkReconnect(NULL, mLogic, eventId,
					  this, SLOT(onConnectionEvent(vtkObject*, void*, unsigned long, void*)));
	}
}

void NetworkHandler::connectToDeviceEvents()
{
	foreach(int eventId, QList<int>()
			<< igtlioLogic::NewDeviceEvent
			<< igtlioLogic::RemovedDeviceEvent
			)
	{
		qvtkReconnect(NULL, mLogic, eventId,
					this, SLOT(onDeviceAddedOrRemoved(vtkObject*, void*, unsigned long, void*)));
	}
}

} // namespace cx
