/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegisteredService.h"
#include <iostream>
//#include "cxTypeConversions.h"

namespace cx
{

RegisteredService::RegisteredService(ctkPluginContext* context, QObject* instance, QString iid)
{
	mService.reset(instance);
//	std::cout << "created service " << iid << std::endl;
	try
	{
		mReference= context->registerService(QStringList(iid), mService.get());
	}
	catch(ctkRuntimeException& e)
	{
		std::cout << e.what() << std::endl;
		mService.reset();
	}
//	std::cout << "registered service " << iid << std::endl;
}

RegisteredService::~RegisteredService()
{
	if(mReference)
		mReference.unregister();
	mService.reset();
}
} //namespace cx
