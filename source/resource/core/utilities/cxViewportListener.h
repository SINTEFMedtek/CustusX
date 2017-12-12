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
#ifndef CXVIEWPORTLISTENER_H_
#define CXVIEWPORTLISTENER_H_

#include "cxResourceExport.h"

#include <boost/function.hpp>
#include "vtkForwardDeclarations.h"
#include "cxVector3D.h"

namespace cx
{


/** \brief Base class for listening to a vtkRenderer
 *
 *  Subclass by adding the observer to the mRenderer
 *  A callback function is called when a change is detected.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT ViewportListenerBase
{
public:
		ViewportListenerBase();
		virtual ~ViewportListenerBase();

		void startListen(vtkRendererPtr renderer);
		void stopListen();
		bool isListening() const;

		void setCallback(boost::function<void ()> func);

		virtual void callback();
		double getVpnZoom();
		double getVpnZoom(Vector3D focusPoint);
protected:
		virtual void addObservers() = 0;
		virtual void removeObservers() = 0;

		typedef vtkSmartPointer<class ViewportObserverPrivate> ViewportObserverPrivatePtr;
		ViewportObserverPrivatePtr mObserver;
		vtkRendererPtr mRenderer;
private:
		boost::function<void ()> mCallback;
};

/** \brief Listens to changes in viewport and camera matrix.
 *
 *  Class that listens to changes in the viewport size and camera matrix.
 *  A callback function is called when a change is detected.
 *
 *  This can be used to update reps with respect to viewport size, for example
 *  keep a 3D object in constant observed size.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT ViewportListener : public ViewportListenerBase
{
public:
		ViewportListener() {}
		virtual ~ViewportListener();
		void setModified();

protected:
		virtual void addObservers();
		virtual void removeObservers();
};

typedef boost::shared_ptr<class ViewportListener> ViewportListenerPtr;

/** \brief Listens to the start render event in a vtkRenderer
 *
 *  A callback function is called when render is about to start.
 *  Use to perform special processing prior to a render operation.
 *
 * The callback is called only of setModified() has been called since
 * last callback.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT ViewportPreRenderListener : public ViewportListenerBase
{
public:
		ViewportPreRenderListener();
		virtual ~ViewportPreRenderListener();
		virtual void callback();
		void setModified();

protected:
		virtual void addObservers();
		virtual void removeObservers();
private:
		bool mModified;
};

typedef boost::shared_ptr<class ViewportPreRenderListener> ViewportPreRenderListenerPtr;

}

#endif /* CXVIEWPORTLISTENER_H_ */
