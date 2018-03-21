/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStreamerServiceUtilities.h"

#include <QList>
#include <ctkServiceTracker.h>
#include <iostream>
#include "cxTypeConversions.h"

namespace cx
{
StreamerService* StreamerServiceUtilities::getStreamerServiceFromType(QString name, ctkPluginContext* context)
{
	ctkServiceTracker<StreamerService*> tracker(context);
	tracker.open();

	QList<StreamerService*> serviceList = tracker.getServices();

	for(int i = 0; i < serviceList.size(); ++i)
	{
		StreamerService* service = serviceList.at(i);
		if (service->getType() == name)
			return service;
	}

	return NULL;
}
} /* namespace cx */
