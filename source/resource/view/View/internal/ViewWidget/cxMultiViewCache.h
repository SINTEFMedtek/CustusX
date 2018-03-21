/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

class cxResourceVisualization_EXPORT MultiViewCache
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
