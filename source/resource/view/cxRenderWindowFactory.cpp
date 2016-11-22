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
#include "vtkRenderWindow.h"

namespace cx
{


RenderWindowFactory::RenderWindowFactory()
{
    mSharedRenderWindow = createRenderWindow(false);//Setting offScreenRendering to true gives crash in render
    mSharedRenderWindow->Render();//Crash
}

vtkRenderWindowPtr RenderWindowFactory::getRenderWindow(QString uid, bool offScreenRendering)
{
    if (mRenderWindows.count(uid))
        return mRenderWindows[uid];

    vtkRenderWindowPtr renderWindow = this->createRenderWindow(offScreenRendering);
//    std::map<QString, vtkRenderWindowPtr> iter = mRenderWindows.begin();
    mRenderWindows[uid] = renderWindow;
    return renderWindow;
}

vtkRenderWindowPtr RenderWindowFactory::getSharedRenderWindow() const
{
//    mSharedRenderWindow->Render();//Crash?
    return mSharedRenderWindow;
}

vtkRenderWindowPtr RenderWindowFactory::createRenderWindow(bool offScreenRendering)
{
	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	renderWindow->SetOffScreenRendering(offScreenRendering);
	return renderWindow;
}

}//cx