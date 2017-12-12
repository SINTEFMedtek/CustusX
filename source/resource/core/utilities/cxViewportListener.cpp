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
#include "cxViewportListener.h"
#include "vtkRenderer.h"
#include "cxVector3D.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "cxLogger.h"

namespace cx
{

class ViewportObserverPrivate : public vtkCommand
{
public:
	ViewportObserverPrivate() : mBase(NULL) {}
	static ViewportObserverPrivate* New() {return new ViewportObserverPrivate;}
	void SetBase(ViewportListenerBase* base) {mBase = base;}
	virtual void Execute(vtkObject* caller, unsigned long, void*)
	{
		if (mBase)
			mBase->callback();
	}
	ViewportListenerBase* mBase;
};


ViewportListenerBase::ViewportListenerBase()
{

}

ViewportListenerBase::~ViewportListenerBase()
{
//	this->stopListen();
}

void ViewportListenerBase::startListen(vtkRendererPtr renderer)
{
	mRenderer = renderer;

	// turn on observer
	if (!mObserver)
	{
		mObserver = ViewportObserverPrivatePtr::New();
		mObserver->SetBase(this);
		this->addObservers();
//		mRenderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, mObserver); // needed during startup
//		mRenderer->AddObserver(vtkCommand::ModifiedEvent, mObserver); // camera changes, viewport changes
//		mRenderer->AddObserver(vtkCommand::ActiveCameraEvent, mObserver);
//		mRenderer->AddObserver(vtkCommand::ResetCameraEvent, mObserver);
		//	  mRenderer->AddObserver(vtkCommand::CreateCameraEvent, mObserver);
		//	  mRenderer->AddObserver(vtkCommand::LeaveEvent, mObserver);
	}
}


void ViewportListenerBase::stopListen()
{
	// turn off observer
	if (mObserver)
	{
		mObserver->SetBase(NULL);
		this->removeObservers();

//		mRenderer->GetActiveCamera()->RemoveObserver(mObserver);
//		mRenderer->RemoveObserver(mObserver);

//		if (mRenderer->GetActiveCamera()->HasObserver(vtkCommand::ModifiedEvent, mObserver))
//			std::cout << "ERROR camera vtkCommand::ModifiedEvent" << std::endl;
//		if (mRenderer->HasObserver(vtkCommand::ModifiedEvent, mObserver))
//			std::cout << "ERROR vtkCommand::ModifiedEvent" << std::endl;
//		if (mRenderer->HasObserver(vtkCommand::ActiveCameraEvent, mObserver))
//			std::cout << "ERROR vtkCommand::ActiveCameraEvent" << std::endl;
//		if (mRenderer->HasObserver(vtkCommand::ResetCameraEvent, mObserver))
//			std::cout << "ERROR vtkCommand::ResetCameraEvent" << std::endl;

		mObserver = 0;
	}
}

bool ViewportListenerBase::isListening() const
{
	return mObserver!=0;
}

void ViewportListenerBase::setCallback(boost::function<void ()> func)
{
	mCallback = func;
}

void ViewportListenerBase::callback()
{
	if (mCallback)
		mCallback();
}

/**Calculate the size of a 1mm line at the focal point projected into
 * the normalized viewport space.
 *
 */
double ViewportListenerBase::getVpnZoom()
{
	vtkCameraPtr camera = mRenderer->GetActiveCamera();
	Vector3D p_f(camera->GetFocalPoint());
	return this->getVpnZoom(p_f);
}

double ViewportListenerBase::getVpnZoom(Vector3D focusPoint)
{
	if (!mRenderer)
	{
		reportError("No renderer set, using zoom=1");
		return 1;
	}
	//  use the focal point and focal point + vup.
	//  Transform both to view space and remove z-coord.
	//  The distance between then in the view plane can
	//  be used to rescale the text.
	vtkCameraPtr camera = mRenderer->GetActiveCamera();
	Vector3D p_f(focusPoint);
	Vector3D vup(camera->GetViewUp());
	Vector3D p_fup = p_f+vup;
	//CX_LOG_DEBUG() << "p_f: " << p_f << " vup: " << vup << " p_fup: " << p_fup;

	mRenderer->WorldToView(p_f[0],p_f[1],p_f[2]);
	mRenderer->WorldToView(p_fup[0],p_fup[1],p_fup[2]);
	p_f[2] = 0;
	p_fup[2] = 0;
	//CX_LOG_DEBUG() << "p_f: " << p_f << " vup: " << vup << " p_fup: " << p_fup;
	double size = (p_f - p_fup).length()/2.0;
	return size;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

void ViewportListener::addObservers()
{
	mRenderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, mObserver); // needed during startup
	mRenderer->AddObserver(vtkCommand::ModifiedEvent, mObserver); // camera changes, viewport changes
	mRenderer->AddObserver(vtkCommand::ActiveCameraEvent, mObserver);
	mRenderer->AddObserver(vtkCommand::ResetCameraEvent, mObserver);
}
void ViewportListener::removeObservers()
{
	mRenderer->GetActiveCamera()->RemoveObserver(mObserver);
	mRenderer->RemoveObserver(mObserver);
}

ViewportListener::~ViewportListener()
{
	this->stopListen();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ViewportPreRenderListener::ViewportPreRenderListener() : mModified(true)
{
}

void ViewportPreRenderListener::setModified()
{
	mModified = true;
}

void ViewportPreRenderListener::callback()
{
	if (!mModified)
		return;
	ViewportListenerBase::callback();
	mModified = false;
}

void ViewportPreRenderListener::addObservers()
{
	mRenderer->AddObserver(vtkCommand::StartEvent, mObserver, 1.0);
}
void ViewportPreRenderListener::removeObservers()
{
	mRenderer->RemoveObserver(mObserver);
}

ViewportPreRenderListener::~ViewportPreRenderListener()
{
	this->stopListen();
}

}
