// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

/*
 * cxViewCache.h
 *
 *  \date Jul 29, 2010
 *      \author christiana
 */

#ifndef CXVIEWCACHE_H_
#define CXVIEWCACHE_H_

#include <QWidget>
#include <QLayout>
#include <vector>
#include "sscTypeConversions.h"
#include "cxSettings.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

/**Cache for reuse of Views.
 * Use the retrieve*() method to get views that can be used
 * in layouts. You will get unique views for each call.
 * When rebuilding the gui, remove all views from their layouts,
 * and call clearUsedViews(). The cache will assume all views now
 * are free and ready for reuse.
 *
 */
template<class VIEW_TYPE>
class ViewCache
{
public:
	ViewCache(QWidget* widget, QString typeText) :
					mCentralWidget(widget), mNameGenerator(0), mTypeText(typeText)
	{
	}
	/**Retrieve a view that is unique since the last call to clearUsedViews()
	 */
	VIEW_TYPE* retrieveView()
	{
//    mCached.clear();
		if (mCached.empty())
		{
			QString uid = qstring_cast(mTypeText) + "-" + qstring_cast(mNameGenerator++);
			VIEW_TYPE* view = new VIEW_TYPE(uid, uid, mCentralWidget);
			view->setContextMenuPolicy(Qt::CustomContextMenu);
			view->hide();
			//Turn off rendering in vtkRenderWindowInteractor
			view->getRenderWindow()->GetInteractor()->EnableRenderOff();
			//Increase the StillUpdateRate in the vtkRenderWindowInteractor (default is 0.0001 images per second)
			double rate = settings()->value("stillUpdateRate").value<double>();
			view->getRenderWindow()->GetInteractor()->SetStillUpdateRate(rate);
			// Set the same value when moving (seems counterintuitive, but for us, moving isnt really special.
			// The real challenge is updating while the tracking is active, and this uses the still update rate.
			view->getRenderWindow()->GetInteractor()->SetDesiredUpdateRate(rate);
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
		for (unsigned i = 0; i < mUsed.size(); ++i)
		{
			mUsed[i]->hide();
			mCentralWidget->layout()->removeWidget(mUsed[i]);
		}

		std::copy(mUsed.begin(), mUsed.end(), back_inserter(mCached));
		mUsed.clear();
	}
private:
	QWidget* mCentralWidget;
	int mNameGenerator;
	QString mTypeText;
	std::vector<VIEW_TYPE*> mCached;
	std::vector<VIEW_TYPE*> mUsed;
};

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWCACHE_H_ */
