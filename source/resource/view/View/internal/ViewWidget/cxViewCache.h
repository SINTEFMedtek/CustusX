/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWCACHE_H_
#define CXVIEWCACHE_H_

#include "cxResourceVisualizationExport.h"

#include <QWidget>
#include <QLayout>
#include <vector>
#include "cxTypeConversions.h"
#include "cxOSXHelper.h"
#include "cxViewWidget.h"
#include "cxRenderWindowFactory.h"

namespace cx
{
/**
 * \file
 * \ingroup cx_resource_view_internal
 * @{
 */

/**
 * Cache for reuse of Views.
 * Use the retrieve*() method to get views that can be used
 * in layouts. You will get unique views for each call.
 * When rebuilding the gui, remove all views from their layouts,
 * and call clearUsedViews(). The cache will assume all views now
 * are free and ready for reuse.
 *
 * \date 2010-07-29
 * \author christiana
 * \ingroup cx_resource_view_internal
 */
template<class VIEW_TYPE>
class cxResourceVisualization_EXPORT ViewCache
{
public:
	ViewCache(RenderWindowFactoryPtr factory, QWidget* widget, QString typeText) :
		mRenderWindowFactory(factory),
		mCentralWidget(widget),
		mNameGenerator(0),
		mTypeText(typeText)
	{
	}
	/**Retrieve a view that is unique since the last call to clearUsedViews()
	 */
	VIEW_TYPE* retrieveView()
	{
		if (mCached.empty())
		{
			QString uid = QString("%1-%2-%3")
					.arg(mTypeText)
					.arg(mNameGenerator++)
					.arg(reinterpret_cast<long>(this));
			VIEW_TYPE* view = new VIEW_TYPE(mRenderWindowFactory, uid, uid, mCentralWidget);
			mCached.push_back(view);
		}

		VIEW_TYPE* retval = mCached.back();
		mCached.pop_back();
		mUsed.push_back(retval);
		return retval;
	}
	/**Reset the cache for new use.
	 * Remove all used views from the central widget and hide them.
	 */
	void clearUsedViews()
	{
		std::copy(mUsed.begin(), mUsed.end(), back_inserter(mCached));
		mUsed.clear();
	}
	/**
	 * Clear all cached values
	 */
	void clearCache()
	{
		mCached.clear();
		mUsed.clear();
	}

private:
	QWidget* mCentralWidget;
	int mNameGenerator;
	QString mTypeText;
	std::vector<VIEW_TYPE*> mCached;
	std::vector<VIEW_TYPE*> mUsed;
	RenderWindowFactoryPtr mRenderWindowFactory;
};


/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWCACHE_H_ */
