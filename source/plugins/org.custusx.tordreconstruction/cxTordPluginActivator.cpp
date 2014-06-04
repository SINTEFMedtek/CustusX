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
}

TordPluginActivator::~TordPluginActivator()
{

}

void TordPluginActivator::start(ctkPluginContext* context)
{
	this->mContext = context;

	mPlugin.reset(new TordReconstructionService);
	try
	{
		mRegistration = context->registerService(QStringList(ReconstructionService_iid), mPlugin.get());
	}
	catch(ctkRuntimeException& e)
	{
		std::cout << e.what() << std::endl;
		mPlugin.reset();
	}
}

void TordPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.unregister();
	if(mPlugin)
		mPlugin.reset();
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(TordPluginActivator_irrelevant_string, cx::TordPluginActivator)


