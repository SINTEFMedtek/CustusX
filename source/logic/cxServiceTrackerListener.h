#ifndef CXSERVICETRACKERLISTENER_H_
#define CXSERVICETRACKERLISTENER_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "ctkServiceTracker.h"
#include "ctkServiceTrackerCustomizer.h"
#include "cxPluginFramework.h"
#include "cxServiceTrackerCustomizer.h"

namespace cx
{
/**
 * \brief Helper class for listening to services being added, modified and removed.
 *
 * Usage example:
 * For listening to ReconstructionServices being added and removed, but ignoring if they are modified
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
		ServiceTrackerListener(PluginFrameworkManagerPtr pluginFramework,
													 boost::function<void (T*)> serviceAdded,
													 boost::function<void (T*)> serviceModified,
													 boost::function<void (T*)> serviceRemoved)
    {
        boost::shared_ptr<ServiceTrackerCustomizer<T> > customizer(new ServiceTrackerCustomizer<T>);
        mServiceTrackerCustomizer = customizer;
        mServiceTrackerCustomizer->setServiceAddedCallback(serviceAdded);
        mServiceTrackerCustomizer->setServiceModifiedCallback(serviceModified);
        mServiceTrackerCustomizer->setServiceRemovedCallback(serviceRemoved);

        mServiceTracker.reset(new ctkServiceTracker<T*>(pluginFramework->getPluginContext(), mServiceTrackerCustomizer.get()));
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
