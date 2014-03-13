/*
 * sscViewportListener.h
 *
 *  Created on: Jul 28, 2011
 *      Author: christiana
 */

#ifndef CXVIEWPORTLISTENER_H_
#define CXVIEWPORTLISTENER_H_

#include <boost/function.hpp>
#include "vtkForwardDeclarations.h"

namespace cx
{


/** \brief Base class for listening to a vtkRenderer
 *
 *  Subclass by adding the observer to the mRenderer
 *  A callback function is called when a change is detected.
 *
 * \ingroup cx_resource_core_utilities
 */
class ViewportListenerBase
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
class ViewportListener : public ViewportListenerBase
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
class ViewportPreRenderListener : public ViewportListenerBase
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
