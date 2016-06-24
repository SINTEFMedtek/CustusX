#include "cxtestReceiver.h"

#include "cxLogger.h"

#include "vtkSmartPointer.h"

#include "vtkIGTLIOImageDevice.h"
#include "vtkIGTLIOCommandDevice.h"
#include "vtkIGTLIOTransformDevice.h"
#include "vtkIGTLIOStatusDevice.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

#include "igtlCommandConverter.h"
#include "igtlImageConverter.h"
#include "igtlTransformConverter.h"
#include "igtlStatusConverter.h"


namespace cxtest {

Receiver::Receiver(vtkIGTLIOLogicPointer logic) :
	mEventsReceived(0),
	image_received(false),
	transform_received(false)
{
	mNetwork = new cx::NetworkHandler(logic);
	QObject::connect(mNetwork, &cx::NetworkHandler::image, this, &Receiver::checkImage);
	QObject::connect(mNetwork, &cx::NetworkHandler::transform, this, &Receiver::checkTransform);
}

Receiver::~Receiver()
{
	delete mNetwork;
}

void Receiver::listen(vtkIGTLIODevicePointer device)
{
	CX_LOG_DEBUG() << "Listening to a device: " << device->GetDeviceName();
	qvtkReconnect(NULL, device, vtkIGTLIODevice::ModifiedEvent, this, SLOT(onDeviceModified(vtkObject*, void*, unsigned long, void*)));

}

void Receiver::onDeviceModified(vtkObject* caller, void* device, unsigned long event , void*)
{

	vtkSmartPointer<vtkIGTLIODevice> receivedDevice(reinterpret_cast<vtkIGTLIODevice*>(caller));
	REQUIRE(receivedDevice);
	CX_LOG_DEBUG() << "Received event " << event
				   << " from " << receivedDevice->GetDeviceName()
				   << " which is of type " << receivedDevice->GetDeviceType();

	igtl::BaseConverter::HeaderData header = receivedDevice->GetHeader();
	CX_LOG_DEBUG() << "HEADER: " << " devicename: " << header.deviceName
								<< " timestamp: " << header.timestamp;


	std::string device_type = receivedDevice->GetDeviceType();
	if(device_type == igtl::CommandConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<vtkIGTLIOCommandDevice> command = vtkIGTLIOCommandDevice::SafeDownCast(receivedDevice);
		REQUIRE(command);

		igtl::CommandConverter::ContentData content = command->GetContent();
		CX_LOG_DEBUG() << "COMMAND: "	<< " id: " << content.id
										<< " name: " << content.name
										<< " content: " << content.content;

	}
	else if(device_type == igtl::StatusConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<vtkIGTLIOStatusDevice> status = vtkIGTLIOStatusDevice::SafeDownCast(receivedDevice);
		REQUIRE(status);

		igtl::StatusConverter::ContentData content = status->GetContent();
		CX_LOG_DEBUG() << "STATUS: "	<< " code: " << content.code
										<< " subcode: " << content.subcode
										<< " errorname: " << content.errorname //errorname is an optional field, will only be filled when there is an error
										<< " statusstring: " << content.statusstring;

	}
	else if(device_type == igtl::ImageConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<vtkIGTLIOImageDevice> image = vtkIGTLIOImageDevice::SafeDownCast(receivedDevice);
		REQUIRE(image);

		igtl::ImageConverter::ContentData content = image->GetContent();
		CX_LOG_DEBUG() << "IMAGE: "	<< " image class name: " << content.image->GetClassName()
										<< " transform: " << content.transform;
	}
	else if(device_type == igtl::TransformConverter::GetIGTLTypeName())
	{
		vtkSmartPointer<vtkIGTLIOTransformDevice> transform = vtkIGTLIOTransformDevice::SafeDownCast(receivedDevice);
		REQUIRE(transform);

		igtl::TransformConverter::ContentData content = transform->GetContent();
		CX_LOG_DEBUG() << "TRANSFORM: "	<< " transform: " << content.transform
										<< " deviceName: " << content.deviceName;

	}
	else
	{
		INFO("Receiving unknown device type.");
		REQUIRE(false);
	}

	mEventsReceived += 1;
	if(mEventsReceived > 10)
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

}//namespace cxtest
