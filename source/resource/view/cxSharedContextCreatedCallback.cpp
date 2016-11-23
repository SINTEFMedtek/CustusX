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

/*
void SharedContextCreatedCallback::setViewService(ViewServicePtr viewService)
{
	mViewService = viewService;
}
*/

void SharedContextCreatedCallback::Execute(vtkObject *renderWindow, unsigned long eventId, void *cbo)
{
	CX_LOG_DEBUG() << "START SharedContextCreatedCallback";
	/*
	if(!mViewService)
	{
		CX_LOG_ERROR() << "SharedContextCreatedCallback::Execute: ViewService missing";
		return;
	}
	*/
	//RenderWindowFactoryPtr factory = boost::dynamic_pointer_cast<RenderWindowFactor>(cbo);
	if(!mRenderWindowFactory)
	{
		CX_LOG_ERROR() << "SharedContextCreatedCallback::Execute: RenderWindowFactoryPtr missing";
		return;
	}
	if(eventId == vtkCommand::CXSharedContextCreatedEvent)
	{
		CX_LOG_DEBUG() << "1 SharedContextCreatedCallback";
	//------------------------------
	//TODO Create centralized storage/factory for creating renderwindows

	// SharedOpenGLContext should be created with a pointer to the first renderwindow created in CustusX
	// because that renderwindow is special, it contain THE shared opengl context
	//if(!this->mSharedOpenGLContext)
	//{
		vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow);
//		vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(mViewService->getSharedRenderWindow());

		CX_LOG_DEBUG() << "2 SharedContextCreatedCallback";
		if(SharedOpenGLContext::isValid(opengl_renderwindow, true))
		{
			CX_LOG_DEBUG() << "3 SharedContextCreatedCallback";
			mRenderWindowFactory->setSharedRenderWindow(opengl_renderwindow);
		}
		else
			CX_LOG_WARNING() << "VTK render window is not an opengl renderwindow. This means we don't have an OpenGL shared context";
	//}
	//------------------------------
	}
	else
		CX_LOG_WARNING() << "BLEH";

	CX_LOG_DEBUG() << "END SharedContextCreatedCallback";
}

}//cx
