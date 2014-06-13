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

#include "cxPNNReconstructionPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxPNNReconstructionService.h"

namespace cx
{

PNNReconstructionPluginActivator::PNNReconstructionPluginActivator()
: mContext(0)
{
}

PNNReconstructionPluginActivator::~PNNReconstructionPluginActivator()
{

}

void PNNReconstructionPluginActivator::start(ctkPluginContext* context)
{
	this->mContext = context;

	mPlugin.reset(new PNNReconstructionService);
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

void PNNReconstructionPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.unregister();
	if(mPlugin)
		mPlugin.reset();
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(PNNReconstructionPluginActivator_irrelevant_string, cx::PNNReconstructionPluginActivator)


