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

#include "cxTordPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxTordReconstructionService.h"

namespace cx
{

TordPluginActivator::TordPluginActivator()
: mContext(0)
{
	std::cout << "Created TordPluginActivator" << std::endl;
}

TordPluginActivator::~TordPluginActivator()
{

}

void TordPluginActivator::start(ctkPluginContext* context)
{
	std::cout << "Started TordPluginActivator" << std::endl;
	this->mContext = context;

	mPlugin.reset(new TordReconstructionService);
	std::cout << "created tordreconstruction service" << std::endl;
	try
	{
		context->registerService(QStringList(ReconstructionService_iid), mPlugin.get());
	}
	catch(ctkRuntimeException& e)
	{
		std::cout << e.what() << std::endl;
		mPlugin.reset();
	}
	std::cout << "registered tordreconstruction service" << std::endl;
}

void TordPluginActivator::stop(ctkPluginContext* context)
{
	mPlugin.reset();
	std::cout << "Stopped TordPluginActivator" << std::endl;
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(TordPluginActivator_irrelevant_string, cx::TordPluginActivator)


