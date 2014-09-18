/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXSERVICETRACKERLISTENER_H_
#define CXSERVICETRACKERLISTENER_H_

#include <QSharedDataPointer>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "ctkServiceTracker.h"
#include "ctkServiceTrackerCustomizer.h"
#include "ctkPluginContext.h"
#include <ctkPluginFramework.h>
#include "cxServiceTrackerCustomizer.h"


namespace cx
{
/**
 * \brief Helper class for listening to services being added, modified and removed.
 *
 * Usage example:
 * For listening to ReconstructionServices being added and removed, but ignoring if they are modified
 *
 * NB: This class can only be used with a valid plugin context.
 * And plugin contexts are only valid when the plugin framework is in one of these states: ACTIVE, STARTING and STOPPING
 *
 *  boost::shared_ptr<ServiceTrackerListener<ReconstructionService> > mServiceListener;
 *  mServiceListener.reset(new ServiceTrackerListener<ReconstructionService>(
 *          LogicManager::getInstance()->getPluginFramework(),
 *          boost::bind(&ReconstructManager::onServiceAdded, this, _1),
 *          boost::bind(&ReconstructManager::onServiceModified, this, _1),
 *          boost::bind(&ReconstructManager::onServiceRemoved, this, _1)
 *  ));
 *
 *
 * \ingroup cx_logic
 * \date May 14, 2014
 * \author Janne Beate Bakeng, SINTEF
 */

template <class T >
class ServiceTrackerListener
{

public:
	ServiceTrackerListener(ctkPluginContext* context,
						   boost::function<void (T*)> serviceAdded,
						   boost::function<void (T*)> serviceModified,
							 boost::function<void (T*)> serviceRemoved)
	{
		boost::shared_ptr<ServiceTrackerCustomizer<T> > customizer(new ServiceTrackerCustomizer<T>);
		mServiceTrackerCustomizer = customizer;
		mServiceTrackerCustomizer->setServiceAddedCallback(serviceAdded);
		mServiceTrackerCustomizer->setServiceModifiedCallback(serviceModified);
		mServiceTrackerCustomizer->setServiceRemovedCallback(serviceRemoved);
		mServiceTracker.reset(new ctkServiceTracker<T*>(context, mServiceTrackerCustomizer.get()));
	}

	~ServiceTrackerListener()
	{
//		mServiceTracker->close();//For some reason this causes a crash if a service (that uses another service with a ServiceTrackerListener) have been removed
		//If close is needed: A possible workasround may be to clear all functions in mServiceTrackerCustomizer?
//		mServiceTracker.reset();
	}

	void open()
	{
		mServiceTracker->open();
	}

	T* getService(QString name)
	{
		QList<T*> services = mServiceTracker->getServices();

		T* service = NULL;
		foreach(T* temp, services)
		{
			QString serviceName = temp->getName();
			if(serviceName.compare(name) == 0)
			{
				service = temp;
			}
		}
		return service;
	}

private:
	boost::shared_ptr<ServiceTrackerCustomizer<T> > mServiceTrackerCustomizer;
	boost::shared_ptr<ctkServiceTracker<T*> > mServiceTracker;
};
}//namespace cx

#endif /* CXSERVICETRACKERLISTENER_H_ */
