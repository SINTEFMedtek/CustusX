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

#ifndef CXMULTIVIEWCACHE_H
#define CXMULTIVIEWCACHE_H

#include "cxResourceVisualizationExport.h"

#include <QWidget>
#include <QLayout>
#include <vector>
#include "cxTypeConversions.h"
#include "cxOSXHelper.h"
#include "cxViewWidget.h"
#include "cxViewCache.h"

namespace cx
{
/**
 * \file
 * \ingroup cx_resource_view_internal
 * @{
 */

/**
 * Cache for reuse of Views.
 *
 * Separate caches exist for each type of view. Offscreen rendering also uses separate
 * caches. The cache also stores a separate first renderwindow in order to handle
 * the shared gl context used by cx.
 *
 * \ingroup cx_resource_view_internal
 */
typedef boost::shared_ptr<class MultiViewCache> MultiViewCachePtr;

class MultiViewCache
{
public:
	static MultiViewCachePtr create(RenderWindowFactoryPtr factory) { return MultiViewCachePtr(new MultiViewCache(factory)); }
	MultiViewCache(RenderWindowFactoryPtr factory);

	ViewWidget* retrieveView(QWidget* widget, View::Type type, bool offScreenRendering);
	void clearViews();
	void clearCache();

private:
	typedef boost::shared_ptr<ViewCache<ViewWidget> > ViewCachePtr;
	std::map<QString, ViewCachePtr> mViewCache;
	vtkRenderWindowPtr mStaticRenderWindow;
	RenderWindowFactoryPtr mRenderWindowFactory;
};

/**
 * @}
 */
} // namespace cx


#endif // CXMULTIVIEWCACHE_H
