#include "cxtestReceiver.h"

#include "cxLogger.h"

#include "vtkSmartPointer.h"

#include "igtlioImageDevice.h"
#include "igtlioCommandDevice.h"
#include "igtlioTransformDevice.h"
#include "igtlioStatusDevice.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

#include "igtlioCommandConverter.h"
#include "igtlioImageConverter.h"
#include "igtlioTransformConverter.h"
#include "igtlioStatusConverter.h"


namespace cxtest {

Receiver::Receiver(igtlio::LogicPointer logic) :
	number_of_events_received(0),
	image_received(false),
	transform_received(false),
	command_received(false)
{
	mNetwork = new cx::NetworkHandler(logic);
	QObject::connect(mNetwork, &cx::NetworkHandler::image, this, &Receiver::checkImage);
	QObject::connect(mNetwork, &cx::NetworkHandler::transform, this, &Receiver::checkTransform);
	QObject::connect(mNetwork, &cx::NetworkHandler::commandRespons, this, &Receiver::checkCommand);
}

Receiver::~Receiver()
{
	delete mNetwork;
}

void Receiver::connect(std::string ip, int port)
{
	mSession = mNetwork->requestConnectToServer(ip, port, igtlio::BLOCKING);
}

void Receiver::listen(igtlio::DevicePointer device, bool verbose)
{
	QString deviceName(device->GetDeviceName().c_str());
	CX_LOG_DEBUG() << "Listening to a device: " << deviceName;
	if(verbose)
	{
		qvtkReconnect(NULL, device, igtlio::Device::ModifiedEvent, this, SLOT(onDeviceModifiedPrint(vtkObject*, void*, unsigned long, void*)));
	}
	qvtkReconnect(NULL, device, igtlio::Device::ModifiedEvent, this, SLOT(onDeviceModifiedCount(vtkObject*, void*, unsigned long, void*)));

}

void Receiver::sendCommand()
{
	vtkSmartPointer<igtlio::CommandDevice> device;
	device = mSession->SendCommandQuery("jb_0",
						"GetCapabilities",
						"Jannis");
	CX_LOG_DEBUG() << "*** Sent message from Client to Server";
}

void Receiver::onDeviceModifiedPrint(vtkObject* caller, void* device, unsigned long event , void*)
{
	vtkSmartPointer<igtlio::Device> receivedDevice(reinterpret_cast<igtlio::Device*>(caller));
	REQUIRE(receivedDevice);
	CX_LOG_DEBUG() << "\n\n *** Received event " << event
				   << " from " << receivedDevice->GetDeviceName()
				   << " which is of type " << receivedDevice->GetDeviceType()
				   << " ***";

	igtlio::BaseConverter::HeaderData header = receivedDevice->GetHeader();
	CX_LOG_DEBUG() << "HEADER: " << " devicename: " << header.deviceName << " timestamp: " << header.timestamp;


	std::string device_type = receivedDevice->GetDeviceType();
	if(device_type == igtlio::CommandConverter::GetIGTLTypeName())
	{
		igtlio::CommandDevicePointer command = igtlio::CommandDevice::SafeDownCast(receivedDevice);
		REQUIRE(command);

		igtlio::CommandConverter::ContentData content = command->GetContent();
		CX_LOG_DEBUG() << "COMMAND: "	<< " id: " << content.id
						<< " name: " << content.name
						<< " content: " << content.content;

	}
	else if(device_type == igtlio::StatusConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<igtlio::StatusDevice> status = igtlio::StatusDevice::SafeDownCast(receivedDevice);
		REQUIRE(status);

		igtlio::StatusConverter::ContentData content = status->GetContent();
		CX_LOG_DEBUG() << "STATUS: "	<< " code: " << content.code
						<< " subcode: " << content.subcode
						<< " errorname: " << content.errorname //errorname is an optional field, will only be filled when there is an error
						<< " statusstring: " << content.statusstring;

	}
	else if(device_type == igtlio::ImageConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<igtlio::ImageDevice> image = igtlio::ImageDevice::SafeDownCast(receivedDevice);
		REQUIRE(image);

		igtlio::ImageConverter::ContentData content = image->GetContent();
		CX_LOG_DEBUG() << "IMAGE: "	<< " image class name: " << content.image->GetClassName()
										<< " transform: " << content.transform;
	}
	else if(device_type == igtlio::TransformConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<igtlio::TransformDevice> transform = igtlio::TransformDevice::SafeDownCast(receivedDevice);
		REQUIRE(transform);

		igtlio::TransformConverter::ContentData content = transform->GetContent();
		CX_LOG_DEBUG() << "TRANSFORM: "	<< " transform: " << content.transform
										<< " deviceName: " << content.deviceName;

	}
	else
	{
		INFO("Receiving unknown device type.");
		REQUIRE(false);
	}
}

void Receiver::onDeviceModifiedCount(vtkObject* caller, void* device, unsigned long event , void*)
{
	number_of_events_received += 1;
	if(number_of_events_received > 10)
	{
		emit done();
	}
}

void Receiver::checkImage(cx::ImagePtr image)
{
	image_received = true;
}

void Receiver::checkTransform(QString devicename, cx::Transform3D transform, double timestamp)
{
	transform_received = true;
}

void Receiver::checkCommand(QString devicename, QString xml)
{
	command_received = true;
}

}//namespace cxtest
