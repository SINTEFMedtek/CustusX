#include "cxtestReceiver.h"

namespace cxtest {

Receiver::Receiver()
{}

Receiver::~Receiver()
{}

void Receiver::listen(vtkIGTLIODevicePointer device)
{
	foreach(int evendId, QList<int>()
			<< vtkIGTLIODevice::DeviceModifiedEvent
			)
	{
		qvtkReconnect(NULL, device, evendId,
					  this, SLOT(onDeviceModified(vtkObject*, void*, unsigned long, void*)));
	}
}

void Receiver::onDeviceModified(vtkObject*, void*, unsigned long, void*)
{
	std::cout << "received" << std::endl;
	CHECK(true);
}

}//namespace cxtest
