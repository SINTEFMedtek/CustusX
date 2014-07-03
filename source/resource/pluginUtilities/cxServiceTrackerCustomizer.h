#ifndef CXSERVICETRACKERCUSTOMIZER_H_
#define CXSERVICETRACKERCUSTOMIZER_H_

#include <boost/function.hpp>

#include "ctkServiceTracker.h"
#include "ctkServiceTrackerCustomizer.h"
#include <iostream>

namespace cx {
/**
* \brief Notify changes in a service lifecycle.
*
* See ServiceTrackerListener.h for convenient usage.
*
* \ingroup cx_logic
* \date 2014-05-13
* \author Janne Beate Bakeng
*/

template<class T >
class ServiceTrackerCustomizer : public ctkServiceTrackerCustomizer<T*>
{
public:
		ServiceTrackerCustomizer(){}
		virtual ~ServiceTrackerCustomizer(){}

   virtual T* addingService(const ctkServiceReference &reference)
   {
       T* service = reference.getPlugin()->getPluginContext()->getService<T>(reference);
	   std::cout << "ServiceTrackerCustomizer: service: " << service << std::endl;
//	   std::cout << "ServiceTrackerCustomizer: service: " << service->getName().toStdString() << std::endl;
	   if(mServiceAddedFunction)
    	   mServiceAddedFunction(service);
       return service;
	 }
   virtual void modifiedService(const ctkServiceReference &reference, T* service)
   {
       if(mServiceModifiedFunction)
           mServiceModifiedFunction(service);
       //TODO when does this happen? untested
	 }
   virtual void removedService(const ctkServiceReference &reference, T* service)
   {
       if(mServiceRemovedFunction)
           mServiceRemovedFunction(service);
       reference.getPlugin()->getPluginContext()->ungetService(reference);
	 }

   void setServiceAddedCallback(boost::function<void (T*)> func)
   {
       mServiceAddedFunction = func;
   }
   void setServiceModifiedCallback(boost::function<void (T*)> func)
   {
       mServiceModifiedFunction = func;
   }
   void setServiceRemovedCallback(boost::function<void (T*)> func)
   {
       mServiceRemovedFunction = func;
   }

private:
   boost::function<void (T*)> mServiceAddedFunction;
   boost::function<void (T*)> mServiceModifiedFunction;
   boost::function<void (T*)> mServiceRemovedFunction;
};
} /* namespace cx */

#endif /* CXSERVICETRACKERCUSTOMIZER_H_ */
