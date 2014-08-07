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

#include "cxGEStreamerPluginActivator.h"

#include <QtPlugin>
//#include <iostream>

#include "cxGEStreamerService.h"

namespace cx
{

GEStreamerPluginActivator::GEStreamerPluginActivator()
: mContext(0)
{
	std::cout << "Created GEStreamerPluginActivator" << std::endl;
}

GEStreamerPluginActivator::~GEStreamerPluginActivator()
{

}

void GEStreamerPluginActivator::start(ctkPluginContext* context)
{
	std::cout << "Started GEStreamerPluginActivator" << std::endl;
	this->mContext = context;

	mPlugin.reset(new GEStreamerService);
	std::cout << "created gestreamer service" << std::endl;
	try
	{
		mRegistration = context->registerService(QStringList(StreamerService_iid), mPlugin.get());
	}
	catch(ctkRuntimeException& e)
	{
		std::cout << e.what() << std::endl;
		mPlugin.reset();
	}
	std::cout << "registered gestreamer service" << std::endl;
}

void GEStreamerPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.unregister();
	if(mPlugin)
		mPlugin.reset();
	std::cout << "Stopped GEStreamerPluginActivator" << std::endl;
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(GEStreamerPluginActivator_irrelevant_string, cx::GEStreamerPluginActivator)


