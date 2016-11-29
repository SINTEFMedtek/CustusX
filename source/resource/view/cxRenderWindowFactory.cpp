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

#include "cxRenderWindowFactory.h"

#include <QString>
#include <vtkRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>
#include "cxSharedOpenGLContext.h"
#include "cxLogger.h"

namespace cx
{


RenderWindowFactory::RenderWindowFactory()
{
	vtkRenderWindowPtr renderWindow = createRenderWindow("cx_shared_context", false);//Setting offScreenRendering to true gives crash in render
	renderWindow->Render();//Crash
}

vtkRenderWindowPtr RenderWindowFactory::getRenderWindow(QString uid, bool offScreenRendering)
{
	if(this->renderWindowExists(uid))
        return mRenderWindows[uid];

	vtkRenderWindowPtr renderWindow = this->createRenderWindow(uid, offScreenRendering);
    return renderWindow;
}

bool RenderWindowFactory::renderWindowExists(QString uid)
{
	return (mRenderWindows.count(uid) > 0);
}

vtkRenderWindowPtr RenderWindowFactory::getSharedRenderWindow() const
{
	CX_LOG_DEBUG() << "RenderWindowFactory::getSharedRenderWindow()";
//    mSharedRenderWindow->Render();//Crash?
	if(!mSharedRenderWindow)
		CX_LOG_WARNING() << "RenderWindowFactory::getSharedRenderWindow(): No shared opengl renderwindow available.";
	return mSharedRenderWindow;
}

void RenderWindowFactory::setSharedRenderWindow(vtkRenderWindowPtr sharedRenderWindow)
{
	if(mSharedRenderWindow == sharedRenderWindow)
		return;
	mSharedRenderWindow = sharedRenderWindow;
	CX_LOG_DEBUG() << "RenderWindowFactory::setSharedRenderWindow(...)\n";
//	mSharedRenderWindow->PrintSelf(std::cout, vtkIndent(10));

	vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(mSharedRenderWindow);
	if(opengl_renderwindow)
	{
		mSharedOpenGLContext = SharedOpenGLContextPtr(new SharedOpenGLContext(opengl_renderwindow));
		CX_LOG_DEBUG_CHECKPOINT();
	}
	else
		CX_LOG_WARNING() << "RenderWindowFactory::setSharedRenderWindow(): got no opengl render window.";
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
	//mSharedContextCreatedCallback->setViewService(mBackend->view());
	mSharedContextCreatedCallback->setRenderWindowFactory(this);

	vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow);
	if(!opengl_renderwindow)
		CX_LOG_ERROR() << "UPS.....";
	opengl_renderwindow->AddObserver(vtkCommand::CXSharedContextCreatedEvent, mSharedContextCreatedCallback);

	mRenderWindows[uid] = renderWindow;

	return renderWindow;
}

}//cx
