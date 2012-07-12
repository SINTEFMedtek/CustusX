/*
 * sscViewportListener.h
 *
 *  Created on: Jul 28, 2011
 *      Author: christiana
 */

#ifndef SSCVIEWPORTLISTENER_H_
#define SSCVIEWPORTLISTENER_H_

#include <boost/function.hpp>
#include "vtkForwardDeclarations.h"

namespace ssc
{


/**\brief Listens to changes in viewport and camera matrix.
 *
 *  Class that listens to changes in the viewport size and camera matrix.
 *  A callback function is called when a change is detected.
 *
 *  This can be used to update reps with respect to viewport size, for example
 *  keep a 3D object in constant observed size.
 *
 * \ingroup sscUtility
 */
class ViewportListener
{
public:
		ViewportListener();
		~ViewportListener();

		void startListen(vtkRendererPtr renderer);
		void stopListen();
		bool isListening() const;

		void setCallback(boost::function<void ()> func);

		void callback();
		double getVpnZoom();

private:
		typedef vtkSmartPointer<class ViewportObserverPrivate> ViewportObserverPrivatePtr;
		ViewportObserverPrivatePtr mObserver;
		vtkRendererPtr mRenderer;
		boost::function<void ()> mCallback;
};

typedef boost::shared_ptr<ViewportListener> ViewportListenerPtr;

}

#endif /* SSCVIEWPORTLISTENER_H_ */
