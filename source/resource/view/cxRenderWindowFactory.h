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

#ifndef CXRENDERWINDOWFACTORY_H
#define CXRENDERWINDOWFACTORY_H


#include "cxResourceVisualizationExport.h"

#include <map>
#include <QString>
#include <boost/smart_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxSharedContextCreatedCallback.h"

namespace cx
{
typedef boost::shared_ptr<class RenderWindowFactory> RenderWindowFactoryPtr;

/** \brief Use to create all vtkRenderWindows, and store a single shared render window.
 *
 * Only used directly by ViewService
 *
 *  \ingroup cx_resource_view
 *  \date 2016-11-21
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT RenderWindowFactory : public QObject
{
    Q_OBJECT
public:
    RenderWindowFactory();
	vtkRenderWindowPtr getRenderWindow(QString uid, bool offScreenRendering = false);
    vtkRenderWindowPtr getSharedRenderWindow() const;

    SharedOpenGLContextPtr getSharedOpenGLContext() const;
signals:
    void sharedOpenGLContextCreated(SharedOpenGLContextPtr);

private:
	void setSharedRenderWindow(vtkRenderWindowPtr sharedRenderWindow);
	vtkRenderWindowPtr createRenderWindow(QString uid, bool offScreenRendering);

	vtkRenderWindowPtr mSharedRenderWindow;
	std::map<QString, vtkRenderWindowPtr> mRenderWindows;
	SharedContextCreatedCallbackPtr mSharedContextCreatedCallback;
	SharedOpenGLContextPtr mSharedOpenGLContext;

	friend void SharedContextCreatedCallback::Execute(vtkObject *view, unsigned long eventId, void*cbo);
};
}//cx

#endif // CXRENDERWINDOWFACTORY_H
