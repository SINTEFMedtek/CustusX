#include "cxtestReceiver.h"

#include "cxLogger.h"

namespace cxtest {

Receiver::Receiver() :
	mEventsReceived(0)
{}

Receiver::~Receiver()
{}

void Receiver::listen(vtkIGTLIODevicePointer device)
{
	CX_LOG_DEBUG() << "Listening to a device: " << device->GetDeviceName();
	qvtkReconnect(NULL, device, vtkCommand::ModifiedEvent, this, SLOT(onDeviceModified(vtkObject*, void*, unsigned long, void*)));

}

void Receiver::onDeviceModified(vtkObject*, void*, unsigned long, void*)
{
	mEventsReceived += 1;
	emit done();
}

}//namespace cxtest
