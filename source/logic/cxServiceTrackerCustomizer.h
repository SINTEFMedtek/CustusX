#ifndef CXSERVICETRACKERCUSTOMIZER_H_
#define CXSERVICETRACKERCUSTOMIZER_H_

#include <boost/function.hpp>

#include "ctkServiceTracker.h"
#include "ctkServiceTrackerCustomizer.h"

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
    ServiceTrackerCustomizer(){};
    virtual ~ServiceTrackerCustomizer(){};

   virtual T* addingService(const ctkServiceReference &reference)
   {
       T* service = reference.getPlugin()->getPluginContext()->getService<T>(reference);
       if(mServiceAddedFunction)
           mServiceAddedFunction();
       return service;
   };
   virtual void modifiedService(const ctkServiceReference &reference, T* service)
   {
       if(mServiceModifiedFunction)
           mServiceModifiedFunction();
       //TODO when does this happen? untested
   };
   virtual void removedService(const ctkServiceReference &reference, T* service)
   {
       if(mServiceRemovedFunction)
           mServiceRemovedFunction();
       reference.getPlugin()->getPluginContext()->ungetService(reference);
   };

   void setServiceAddedCallback(boost::function<void ()> func)
   {
       mServiceAddedFunction = func;
   }
   void setServiceModifiedCallback(boost::function<void ()> func)
   {
       mServiceModifiedFunction = func;
   }
   void setServiceRemovedCallback(boost::function<void ()> func)
   {
       mServiceRemovedFunction = func;
   }

private:
   boost::function<void ()> mServiceAddedFunction;
   boost::function<void ()> mServiceModifiedFunction;
   boost::function<void ()> mServiceRemovedFunction;
};
} /* namespace cx */

#endif /* CXSERVICETRACKERCUSTOMIZER_H_ */
