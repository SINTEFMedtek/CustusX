/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSharedContextCreatedCallback.h"

#include <vtkOpenGLRenderWindow.h>

#include "vtkRenderWindow.h"
#include "cxLogger.h"
#include "cxSharedOpenGLContext.h"
#include "cxRenderWindowFactory.h"

namespace cx
{

SharedContextCreatedCallback *SharedContextCreatedCallback::New()
{
	return new SharedContextCreatedCallback();
}

SharedContextCreatedCallback::SharedContextCreatedCallback() :
	mRenderWindowFactory(NULL)
{
}

void SharedContextCreatedCallback::setRenderWindowFactory(RenderWindowFactory *factory)
{
	mRenderWindowFactory = factory;
}

void SharedContextCreatedCallback::Execute(vtkObject *renderWindow, unsigned long eventId, void *cbo)
{
	if(!mRenderWindowFactory)
	{
		CX_LOG_ERROR() << "SharedContextCreatedCallback::Execute: RenderWindowFactoryPtr missing";
		return;
	}

	if(eventId == vtkCommand::CXSharedContextCreatedEvent)
	{
		vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow);

		if(SharedOpenGLContext::isValid(opengl_renderwindow))
		{
			mRenderWindowFactory->setSharedRenderWindow(opengl_renderwindow);
		}
		else
		{
			CX_LOG_WARNING() << "VTK render window is not an opengl renderwindow. This means we don't have an OpenGL shared context";
		}
	}
}

}//cx
