/*
 * sscViewportListener.cpp
 *
 *  Created on: Jul 28, 2011
 *      Author: christiana
 */

#include <sscViewportListener.h>
#include "vtkRenderer.h"
#include "sscVector3D.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "sscMessageManager.h"

namespace ssc
{

class ViewportObserverPrivate : public vtkCommand
{
public:
	ViewportObserverPrivate() : mBase(NULL) {}
	static ViewportObserverPrivate* New() {return new ViewportObserverPrivate;}
	void SetBase(ViewportListener* base) {mBase = base;}
	virtual void Execute(vtkObject* caller, unsigned long, void*)
	{
		if (mBase)
			mBase->callback();
	}
	ViewportListener* mBase;
};


ViewportListener::ViewportListener()
{

}

ViewportListener::~ViewportListener()
{
	this->stopListen();
}

void ViewportListener::startListen(vtkRendererPtr renderer)
{
	mRenderer = renderer;

	// turn on observer
	if (!mObserver)
	{
		mObserver = ViewportObserverPrivatePtr::New();
		mObserver->SetBase(this);
		mRenderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, mObserver); // needed during startup
	  mRenderer->AddObserver(vtkCommand::ModifiedEvent, mObserver); // camera changes, viewport changes
	  mRenderer->AddObserver(vtkCommand::ActiveCameraEvent, mObserver);
	  mRenderer->AddObserver(vtkCommand::ResetCameraEvent, mObserver);
//	  mRenderer->AddObserver(vtkCommand::CreateCameraEvent, mObserver);
//	  mRenderer->AddObserver(vtkCommand::LeaveEvent, mObserver);

	}
}

void ViewportListener::stopListen()
{
	// turn off observer
	if (mObserver)
	{
		mObserver->SetBase(NULL);
		mRenderer->RemoveObserver(mObserver);
		mObserver = 0;
	}
}

bool ViewportListener::isListening() const
{
	return mObserver!=0;
}

void ViewportListener::setCallback(boost::function<void ()> func)
{
	mCallback = func;
}

void ViewportListener::callback()
{
	if (mCallback)
		mCallback();
}

/**Calculate the size of a 1mm line at the focal point projected into
 * the normalized viewport space.
 *
 */
double ViewportListener::getVpnZoom()
{
	if (!mRenderer)
	{
		ssc::messageManager()->sendError("No renderer set, using zoom=1");
		return 1;
	}
	//  use the focal point and focal point + vup.
	//  Transform both to view space and remove z-coord.
	//  The distance between then in the view plane can
	//  be used to rescale the text.
	vtkCameraPtr camera = mRenderer->GetActiveCamera();
	ssc::Vector3D p_f(camera->GetFocalPoint());
	ssc::Vector3D vup(camera->GetViewUp());
	ssc::Vector3D p_fup = p_f+vup;
	mRenderer->WorldToView(p_f[0],p_f[1],p_f[2]);
	mRenderer->WorldToView(p_fup[0],p_fup[1],p_fup[2]);
	p_f[2] = 0;
	p_fup[2] = 0;
	double size = (p_f - p_fup).length()/2;
	return size;
}

}
