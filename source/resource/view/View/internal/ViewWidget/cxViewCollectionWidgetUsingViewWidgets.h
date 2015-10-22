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

#ifndef CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_
#define CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_

#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewCache.h"
#include "cxViewWidget.h"
#include "cxViewCollectionWidget.h"

class QGridLayout;

namespace cx
{

/**
 * Widget for displaying Views, Containing a QGridLayout of QVTKWidgets,
 * one for each view.
 *
 * \ingroup cx_resource_view_internal
 * \date 2013-11-05
 * \author Christian Askeland
 */
class LayoutWidgetUsingViewWidgets : public ViewCollectionWidget
{
	Q_OBJECT
public:
	LayoutWidgetUsingViewWidgets(QWidget *parent);
    virtual ~LayoutWidgetUsingViewWidgets();

	virtual ViewPtr addView(View::Type type, LayoutRegion region);
	virtual void clearViews();
	virtual void setModified();
	virtual void render();
	virtual void setGridSpacing(int val);
	virtual void setGridMargin(int val);
    virtual int getGridSpacing() const;
    virtual int getGridMargin() const;
    virtual std::vector<ViewPtr> getViews();
	virtual LayoutRegion getLayoutRegion(QString view);
    virtual QPoint getPosition(ViewPtr view);

private:
	ViewWidget* retrieveView(View::Type type);
    ViewWidget* WidgetFromView(ViewPtr view);

	boost::shared_ptr<ViewCache<ViewWidget> > mViewCache2D;
	boost::shared_ptr<ViewCache<ViewWidget> > mViewCache3D;
	boost::shared_ptr<ViewCache<ViewWidget> > mViewCacheRT;
	boost::shared_ptr<ViewCache<ViewWidget> > mViewCache;
	QGridLayout* mLayout; ///< the layout
	std::map<QString, LayoutRegion> mRegions;
	std::vector<ViewWidget*> mViews;
};


} // namespace cx

#endif /* CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_ */
