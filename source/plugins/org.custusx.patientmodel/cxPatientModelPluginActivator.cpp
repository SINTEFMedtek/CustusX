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

#include "cxPatientModelPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxPatientModelImplService.h"

namespace cx
{

PatientModelPluginActivator::PatientModelPluginActivator()
: mContext(0)
{
}

PatientModelPluginActivator::~PatientModelPluginActivator()
{

}

void PatientModelPluginActivator::start(ctkPluginContext* context)
{
	this->mContext = context;

	mPatientModelService.reset(new cx::PatientModelImplService);
	try
	{
		context->registerService(QStringList(PatientModelService_iid), mPatientModelService.get());
	}
	catch(ctkRuntimeException& e)
	{
		std::cout << e.what() << std::endl;
		mPatientModelService.reset();
	}
}

void PatientModelPluginActivator::stop(ctkPluginContext* context)
{
	mPatientModelService.reset();
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(PatientModelPluginActivator_irrelevant_string, cx::PatientModelPluginActivator)


