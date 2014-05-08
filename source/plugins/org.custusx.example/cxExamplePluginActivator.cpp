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

#include "cxExamplePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxExampleGUIExtenderService.h"

namespace cx
{

ExamplePluginActivator::ExamplePluginActivator()
: mContext(0)
{
	std::cout << "Created ExamplePluginActivator" << std::endl;
}

ExamplePluginActivator::~ExamplePluginActivator()
{

}

void ExamplePluginActivator::start(ctkPluginContext* context)
{
	std::cout << "Started ExamplePluginActivator" << std::endl;
	this->mContext = context;

	mExamplePlugin.reset(new ExampleGUIExtenderService);
	std::cout << "created dicomplugin service" << std::endl;
	try
	{
		context->registerService(QStringList(GUIExtenderService_iid), mExamplePlugin.get());
	}
	catch(ctkRuntimeException& e)
	{
		std::cout << e.what() << std::endl;
		mExamplePlugin.reset();
	}
	std::cout << "registered dicomplugin service" << std::endl;
}

void ExamplePluginActivator::stop(ctkPluginContext* context)
{
	mExamplePlugin.reset();
	std::cout << "Stopped ExamplePluginActivator" << std::endl;
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(ExamplePluginActivator_irrelevant_string, cx::ExamplePluginActivator)


