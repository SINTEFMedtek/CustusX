/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRenderWindowFactory.h"

#include <QString>
#include <vtkRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>
#include "QVTKWidget.h"
#include "cxSharedOpenGLContext.h"
#include "cxLogger.h"

namespace cx
{
bool RenderWindowFactory::mInstanceExisting = false;


RenderWindowFactory::RenderWindowFactory()
{
	if(mInstanceExisting)
		CX_LOG_ERROR() << "There can only be one instance of RenderWindowFactory <--------";
	else
		mInstanceExisting = true;

	//Note: Setting offScreenRendering to true gives crash in render
	vtkRenderWindowPtr renderWindow = createRenderWindow("cx_shared_context", false);
	this->preventSharedContextRenderWindowFromBeingShownOnScreen(renderWindow);
	renderWindow->Render();
}

void RenderWindowFactory::preventSharedContextRenderWindowFromBeingShownOnScreen(vtkRenderWindowPtr renderWindow)
{
	mQvtkWidgetForHidingSharedContextRenderWindow = new QVTKWidget();
	mQvtkWidgetForHidingSharedContextRenderWindow->SetRenderWindow(renderWindow);
}

vtkRenderWindowPtr RenderWindowFactory::getRenderWindow(QString uid, bool offScreenRendering)
{
	if(this->renderWindowExists(uid))
	{
		return mRenderWindows[uid];
	}

	vtkRenderWindowPtr renderWindow = this->createRenderWindow(uid, offScreenRendering);
	return renderWindow;
}

bool RenderWindowFactory::renderWindowExists(QString uid)
{
	return (mRenderWindows.count(uid) > 0);
}

vtkRenderWindowPtr RenderWindowFactory::getSharedRenderWindow() const
{
	if(!mSharedRenderWindow)
	{
		CX_LOG_WARNING() << "RenderWindowFactory::getSharedRenderWindow(): No shared opengl renderwindow available.";
	}
	return mSharedRenderWindow;
}

void RenderWindowFactory::setSharedRenderWindow(vtkRenderWindowPtr sharedRenderWindow)
{
	if(mSharedRenderWindow == sharedRenderWindow)
	{
		return;
	}
	mSharedRenderWindow = sharedRenderWindow;

	vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(mSharedRenderWindow);
	if(opengl_renderwindow)
	{
		mSharedOpenGLContext = SharedOpenGLContextPtr(new SharedOpenGLContext(opengl_renderwindow));
	}
	else
	{
		CX_LOG_WARNING() << "RenderWindowFactory::setSharedRenderWindow(): got no opengl render window.";
	}
}

SharedOpenGLContextPtr RenderWindowFactory::getSharedOpenGLContext() const
{
	return mSharedOpenGLContext;
}

vtkRenderWindowPtr RenderWindowFactory::createRenderWindow(QString uid, bool offScreenRendering)
{
	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	renderWindow->SetOffScreenRendering(offScreenRendering);

	mSharedContextCreatedCallback = SharedContextCreatedCallbackPtr::New();
	mSharedContextCreatedCallback->setRenderWindowFactory(this);

	vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow);
	if(!opengl_renderwindow)
	{
		CX_LOG_ERROR() << "The created renderwindow is not an opengl renderwindow.";
	}
	opengl_renderwindow->AddObserver(vtkCommand::CXSharedContextCreatedEvent, mSharedContextCreatedCallback);

	mRenderWindows[uid] = renderWindow;

	return renderWindow;
}

}//cx
