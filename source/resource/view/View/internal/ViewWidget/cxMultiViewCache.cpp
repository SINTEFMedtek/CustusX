/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMultiViewCache.h"
#include "vtkRenderWindow.h"
#include "cxViewService.h"
#include "cxLogger.h"

namespace cx
{

MultiViewCache::MultiViewCache()
{
}

ViewWidget* MultiViewCache::retrieveView(QWidget* widget, View::Type type, bool offScreenRendering)
{
	// create one cache per type. This alleviates cross-settings between 2D and 3D,
	// and also separates on/offscreen rendering, which doesn't mix well.
	QString cache_uid = QString("View_%1_%2").arg(type).arg(offScreenRendering);
//	CX_LOG_DEBUG() << "MultiViewCache::retrieveView: " << cache_uid << " " << type;
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
