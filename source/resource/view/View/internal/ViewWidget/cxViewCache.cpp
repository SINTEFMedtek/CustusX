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

#include "cxViewCache.h"
#include "vtkRenderWindow.h"

namespace cx
{

MultiViewCache::MultiViewCache()
{
	// add a hidden window in order to handle the shared context (ref hack in vtkRenderWindow descendants
	// that add support for shared gl contexts)
	if (!mStaticRenderWindow.GetPointer())
	{
		mStaticRenderWindow = vtkRenderWindowPtr::New();
		mStaticRenderWindow->SetOffScreenRendering(true);
		mStaticRenderWindow->Render();
	}
}

ViewWidget* MultiViewCache::retrieveView(QWidget* widget, View::Type type, bool offScreenRendering)
{
	// create one cache per type. This alleviates cross-settings between 2D and 3D,
	// and also separates on/offscreen rendering, which doesn't mix well.
	QString cache_uid = QString("View_%1_%2").arg(type).arg(offScreenRendering);
	if (!mViewCache.count(cache_uid))
		mViewCache[cache_uid].reset(new ViewCache<ViewWidget>(widget, cache_uid));
	ViewCachePtr cache = mViewCache[cache_uid];

	ViewWidget* vw = cache->retrieveView();
	vw->getRenderWindow()->SetOffScreenRendering(offScreenRendering);
	return vw;
}

void MultiViewCache::clearViews()
{
	for (std::map<QString, ViewCachePtr>::iterator iter=mViewCache.begin(); iter!=mViewCache.end(); ++iter)
	{
		iter->second->clearUsedViews();
	}
}

void MultiViewCache::clearCache()
{
	for (std::map<QString, ViewCachePtr>::iterator iter=mViewCache.begin(); iter!=mViewCache.end(); ++iter)
	{
		iter->second->clearCache();
	}

}


} // namespace cx
