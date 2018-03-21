/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSERVICETRACKERLISTENER_H_
#define CXSERVICETRACKERLISTENER_H_

#include <QSharedDataPointer>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#undef REGISTERED //Needed on windows to avoid compiler error. Not sure why.
#include <ctkServiceEvent.h>
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

	T* getServiceFromName(QString name)
	{
		QList<T*> services = mServiceTracker->getServices();
		foreach(T* temp, services)
			if(temp->getName() == name)
				return temp;
		return NULL;
	}

	QList<T*> getServices()
	{
		return mServiceTracker->getServices();
	}

private:
	boost::shared_ptr<ServiceTrackerCustomizer<T> > mServiceTrackerCustomizer;
	boost::shared_ptr<ctkServiceTracker<T*> > mServiceTracker;
};
}//namespace cx

#endif /* CXSERVICETRACKERLISTENER_H_ */
