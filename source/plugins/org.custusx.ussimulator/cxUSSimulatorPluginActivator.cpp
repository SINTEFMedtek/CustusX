// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxUSSimulatorPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxSimulatedImageStreamerService.h"

namespace cx
{

USSimulatorPluginActivator::USSimulatorPluginActivator()
	: mContext(0)
{
	std::cout << "Created USSimulatorPluginActivator" << std::endl;
}

USSimulatorPluginActivator::~USSimulatorPluginActivator()
{

}

void USSimulatorPluginActivator::start(ctkPluginContext* context)
{
	std::cout << "Started USSimulatorPluginActivator" << std::endl;
	this->mContext = context;

	mPlugin.reset(new cx::SimulatedImageStreamerService);
	std::cout << "created ussimulator plugin service" << std::endl;
	try
	{
		context->registerService(QStringList(StreamerService_iid), mPlugin.get());
	}
	catch(ctkRuntimeException& e)
	{
		std::cout << e.what() << std::endl;
		mPlugin.reset();
	}
	std::cout << "registered ussimulator plugin service" << std::endl;
}

void USSimulatorPluginActivator::stop(ctkPluginContext* context)
{
	mPlugin.reset();
	std::cout << "Stopped USSimulatorPluginActivator" << std::endl;
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(USSimulatorPluginActivator_irrelevant_string, cx::USSimulatorPluginActivator)


