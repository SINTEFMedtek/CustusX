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
#include "cxProbeSector.h"
#include "cxProbeDefinition.h"

namespace cx
{

NetworkHandler::NetworkHandler(igtlioLogicPointer logic) :
	mTimer(new QTimer(this)),
	mProbeDefinitionFromStringMessages(ProbeDefinitionFromStringMessagesPtr(new ProbeDefinitionFromStringMessages)),
	mGotTimeOffset(false),
	mTimestampOffsetMS(0),
	mGotMoreThanOneImage(false),
	mProbeDefinition(ProbeDefinitionPtr()),
	mZeroesInImage(true),
	mUSMask(nullptr),
	mSkippedImages(0)
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

void NetworkHandler::clearTimestampSynchronization()
{
	mGotTimeOffset = false;
	mTimestampOffsetMS = 0;
};

double NetworkHandler::synchronizedTimestamp(double receivedTimestampSec)
{
	double receivedTimestampMS = receivedTimestampSec * 1000;
	if(!mGotTimeOffset)
	{
		qint64 systemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
		mTimestampOffsetMS = systemTime - receivedTimestampMS;
		CX_LOG_DEBUG() << "NetworkHandler: Doing timestamp synchronization - adding fixed offset of " << mTimestampOffsetMS << " ms";
		mGotTimeOffset = true;
	}
	receivedTimestampMS = receivedTimestampMS + mTimestampOffsetMS;

	verifyTimestamp(receivedTimestampMS);
	return receivedTimestampMS;
}


bool NetworkHandler::verifyTimestamp(double &timestampMS)
{
	qint64 latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	double diff = timestampMS - latestSystemTime;
	if(fabs(diff) > 1000)
	{
		// BK stops the time whenever the image is frozen, or a parameter is changed
		//CX_LOG_WARNING() << "NetworkHandler: Detected difference between system time and timestamp after synchronization. Difference: " << diff
		//				 << " The reason for this may be messages with different timestamp formats. "
		//				 << " System time will be used instead of received timestamp.";
		timestampMS = latestSystemTime;
		return false;
	}

	return true;
}

void NetworkHandler::onDeviceReceived(vtkObject* caller_device, void* unknown, unsigned long event , void*)
{
	Q_UNUSED(unknown);
	Q_UNUSED(event);
	vtkSmartPointer<igtlioDevice> receivedDevice(reinterpret_cast<igtlioDevice*>(caller_device));

	igtlioBaseConverter::HeaderData header = receivedDevice->GetHeader();
	std::string device_type = receivedDevice->GetDeviceType();

	double timestampMS = synchronizedTimestamp(header.timestamp);


//	CX_LOG_DEBUG() << "Device is modified, device type: " << device_type << " on device: " << receivedDevice->GetDeviceName() << " equipmentId: " << header.equipmentId;

	// Currently the only id available is the Device name defined in Plus xml. Looking like this: Probe_sToReference_s
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
		igtlioLabels << QString("SpacingZ"); //IGTLIO_KEY_SPACING_Z;
		//TODO: Use deciveNameLong when this is defined in IGTLIO and sent with Plus

		mProbeDefinitionFromStringMessages->setImage(cximage);

		for (int i = 0; i < igtlioLabels.size(); ++i)
		{
			metaLabel = igtlioLabels[i].toStdString();
			bool gotMetaData = receivedDevice->GetMetaDataElement(metaLabel, metaDataValue);
			if(!gotMetaData)
			{
				if(!mProbeDefinitionFromStringMessages->haveValidValues() && mGotMoreThanOneImage)
					CX_LOG_WARNING() << "Cannot get needed igtlio meta information: " << metaLabel;
			}
			else
			{
				mProbeDefinitionFromStringMessages->parseValue(metaLabel.c_str(), metaDataValue.c_str());
				//CX_LOG_DEBUG() << "Read variable " << metaLabel << " = " << metaDataValue;
			}
		}
		mGotMoreThanOneImage = true;

		processImageAndEmitProbeDefinition(cximage, deviceName);//Use equipmentId?
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


		// Try to use equipmentId from OpenIGTLink meta data. If not presnet use deviceName.
		// Having equipmentId in OpenIGTLink meta data is something we would like to have a part of the OpenIGTLinkIO standard,
		// and added to the messages from Plus.
		std::string openigtlinktransformid;
		bool gotTransformId = receivedDevice->GetMetaDataElement("equipmentId", openigtlinktransformid);

		if (gotTransformId)
			emit transform(qstring_cast(openigtlinktransformid), cxtransform, timestampMS);
		else
			emit transform(deviceName, cxtransform, timestampMS);
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

		//Allow string messages to modify probe definition, as well as meta info.
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

//TODO: Consider moving these image changing functions out of the class
void NetworkHandler::processImageAndEmitProbeDefinition(ImagePtr cximage, QString deviceName)
{
	bool probeDefinitionHaveChanged = emitProbeDefinitionIfChanged(deviceName);

	if (probeDefinitionHaveChanged)
	{
		mZeroesInImage = true;
		mSkippedImages = 0;
		this->createMask(); //Only create mask once for each probeDefinition
	}

	// Turn off zero conversion if we get a frame without zeroes. Recheck for zeroes every 30 images
	if(mZeroesInImage || (mSkippedImages > 30))
	{
		//			CX_LOG_DEBUG() << "*** Removing zeroes from US image ***";
		mZeroesInImage = convertZeroesInsideSectorToOnes(cximage);
		mSkippedImages = 0;
	}
	else
	{
		++mSkippedImages;
		//			CX_LOG_DEBUG() << "No zeroes in incoming US image";
	}
}

bool NetworkHandler::emitProbeDefinitionIfChanged(QString deviceName)
{
	if (mProbeDefinitionFromStringMessages->haveValidValues() && mProbeDefinitionFromStringMessages->haveChanged())
	{
		mProbeDefinition = mProbeDefinitionFromStringMessages->createProbeDefintion(deviceName);
		emit probedefinition(deviceName, mProbeDefinition);
		return true;
	}
	return false;
}

bool NetworkHandler::convertZeroesInsideSectorToOnes(ImagePtr cximage, int threshold, int newValue)
{
	bool retval = false;
	if(!mUSMask)
		return retval;

	Eigen::Array3i maskDims(mUSMask->GetDimensions());
	unsigned char* maskPtr = static_cast<unsigned char*> (mUSMask->GetScalarPointer());
	unsigned char* imagePtr = static_cast<unsigned char*> (cximage->getBaseVtkImageData()->GetScalarPointer());
	unsigned components = cximage->getBaseVtkImageData()->GetNumberOfScalarComponents();
	unsigned dimX = maskDims[0];
	unsigned dimY = maskDims[1];
	for (unsigned x = 0; x < dimX; x++)
		for (unsigned y = 0; y < dimY; y++)
		{
			unsigned pos = x + y * dimX;
			unsigned imagePos = pos*components;
			unsigned char maskVal = maskPtr[pos];
			unsigned char imageVal = imagePtr[imagePos];
			if (maskVal != 0 && imageVal <= threshold)
			{
				for(unsigned i=0; i < components; ++i)
					if(i < 3) //Only set RGB components, not Alpha
					{
						imagePtr[imagePos + i] = newValue;
						retval = true;
					}
			}
		}
	return retval;
}

bool NetworkHandler::createMask()
{
	if(!mProbeDefinition)
	{
		CX_LOG_WARNING() << "No ProbeDefinition";
		return false;
	}
	ProbeSector probeSector;
	probeSector.setData(*mProbeDefinition.get());

	mUSMask = probeSector.getMask();
	return true;
}

} // namespace cx
