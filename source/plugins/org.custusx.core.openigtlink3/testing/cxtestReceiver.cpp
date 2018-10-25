#include "cxtestReceiver.h"

#include "cxLogger.h"

#include "vtkSmartPointer.h"

#include "igtlioImageDevice.h"
#include "igtlioTransformDevice.h"
#include "igtlioStatusDevice.h"
#include "igtlioStringDevice.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

#include "igtlioCommandConverter.h"
#include "igtlioImageConverter.h"
#include "igtlioTransformConverter.h"
#include "igtlioStatusConverter.h"
#include "igtlioStringConverter.h"


namespace cxtest
{

Receiver::Receiver(igtlioLogicPointer logic) :
	number_of_events_received(0),
	image_received(false),
	transform_received(false),
	command_respons_received(false),
	string_received(false)
{
	mNetwork = new cx::NetworkHandler(logic);
	QObject::connect(mNetwork, &cx::NetworkHandler::image, this, &Receiver::checkImage);
	QObject::connect(mNetwork, &cx::NetworkHandler::transform, this, &Receiver::checkTransform);
	QObject::connect(mNetwork, &cx::NetworkHandler::commandRespons, this, &Receiver::checkCommandRespons);
	QObject::connect(mNetwork, &cx::NetworkHandler::string_message, this, &Receiver::checkString);
}

Receiver::~Receiver()
{
	delete mNetwork;
}

void Receiver::connect(std::string ip, int port)
{
	mSession = mNetwork->requestConnectToServer(ip, port, IGTLIO_BLOCKING);
}

void Receiver::listen(igtlioDevicePointer device, bool verbose)
{
	QString deviceName(device->GetDeviceName().c_str());
	CX_LOG_DEBUG() << "Listening to a device: " << deviceName;
	if(verbose)
	{
		qvtkReconnect(NULL, device, igtlioDevice::ReceiveEvent, this, SLOT(onDeviceModifiedPrint(vtkObject*, void*, unsigned long, void*)));
	}
	qvtkReconnect(NULL, device, igtlioDevice::ReceiveEvent, this, SLOT(onDeviceReceivedCount(vtkObject*, void*, unsigned long, void*)));

}

void Receiver::sendCommand(std::string device_name, std::string command_name, std::string command)
{

//	vtkSmartPointer<igtlioCommandDevice> device;
	/*
	device = mSession->SendCommandQuery("my_device_name",
										"Get",
										"<command> <parameter name=\"Depth\"/> </command>");
										*/

	//device = mSession->SendCommand(device_name, command_name, command
	//									);
	mSession->SendCommand(command_name, command);
	CX_LOG_DEBUG() << "*** Sent message.";
}

void Receiver::sendString()
{
	vtkSmartPointer<igtlioStringDevice> device;
	device = mSession->SendString("my_device_name", "<Command Name=\"RequestChannelIDs\" />");
}

void Receiver::onDeviceModifiedPrint(vtkObject* caller, void* device, unsigned long event, void*)
{
	vtkSmartPointer<igtlioDevice> receivedDevice(reinterpret_cast<igtlioDevice*>(caller));
	REQUIRE(receivedDevice);
	CX_LOG_DEBUG() << "\n\n *** DEVICE IS MODIFIED: " << event
					 << " from " << receivedDevice->GetDeviceName()
					 << " which is of type " << receivedDevice->GetDeviceType()
					 << " ***";

	igtlioBaseConverter::HeaderData header = receivedDevice->GetHeader();
	//CX_LOG_DEBUG() << "HEADER: " << " devicename: " << header.deviceName << " timestamp: " << header.timestamp;


	std::string device_type = receivedDevice->GetDeviceType();
	if(device_type == igtlioStatusConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<igtlioStatusDevice> status = igtlioStatusDevice::SafeDownCast(receivedDevice);
		REQUIRE(status);

		igtlioStatusConverter::ContentData content = status->GetContent();
		/*
		CX_LOG_DEBUG() << "STATUS: "	<< " code: " << content.code
						 << " subcode: " << content.subcode
						 << " errorname: " << content.errorname //errorname is an optional field, will only be filled when there is an error
						 << " statusstring: " << content.statusstring;
		*/

	}
	else if(device_type == igtlioImageConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<igtlioImageDevice> image = igtlioImageDevice::SafeDownCast(receivedDevice);
		REQUIRE(image);

		igtlioImageConverter::ContentData content = image->GetContent();
		/*
		CX_LOG_DEBUG() << "IMAGE: "	<< " image class name: " << content.image->GetClassName()
						 << " transform: " << content.transform;
		*/
	}
	else if(device_type == igtlioTransformConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<igtlioTransformDevice> transform = igtlioTransformDevice::SafeDownCast(receivedDevice);
		REQUIRE(transform);

		igtlioTransformConverter::ContentData content = transform->GetContent();
		/*
		CX_LOG_DEBUG() << "TRANSFORM: "	<< " transform: " << content.transform
						 << " deviceName: " << content.deviceName;
		*/

	}
	else if(device_type == igtlioStringConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<igtlioStringDevice> string = igtlioStringDevice::SafeDownCast(receivedDevice);
		REQUIRE(string);

		igtlioStringConverter::ContentData content = string->GetContent();

		CX_LOG_DEBUG() << "STRING: "	<< " string: " << content.string_msg
						 << " encoding: " << content.encoding;


	}
	else
	{
		INFO("Modified device has unknown device type.");
		REQUIRE(false);
	}
}

void Receiver::onDeviceReceivedCount(vtkObject* caller, void* device, unsigned long event, void*)
{
	number_of_events_received += 1;
}

void Receiver::checkImage(cx::ImagePtr image)
{
	image_received = true;
}

void Receiver::checkTransform(QString devicename, cx::Transform3D transform, double timestamp)
{
	transform_received = true;
}

void Receiver::checkCommandRespons(QString devicename, QString xml)
{
	command_respons_received = true;
}

void Receiver::checkString(QString string)
{
	string_received = true;
//	CX_LOG_DEBUG() << "checkCommand() devicename: " << devicename << " xml: " << xml;
}

}//namespace cxtest
