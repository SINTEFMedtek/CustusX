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
