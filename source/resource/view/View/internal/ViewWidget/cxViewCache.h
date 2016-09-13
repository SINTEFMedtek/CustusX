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

#ifndef CXVIEWCACHE_H_
#define CXVIEWCACHE_H_

#include "cxResourceVisualizationExport.h"

#include <QWidget>
#include <QLayout>
#include <vector>
#include "cxTypeConversions.h"
#include "cxOSXHelper.h"
#include "cxViewWidget.h"

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
	ViewCache(QWidget* widget, QString typeText) :
					mCentralWidget(widget), mNameGenerator(0), mTypeText(typeText)
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
			VIEW_TYPE* view = new VIEW_TYPE(uid, uid, mCentralWidget);
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
};


/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWCACHE_H_ */
