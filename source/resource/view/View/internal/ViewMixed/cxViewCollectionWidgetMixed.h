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

#ifndef CXVIEWCOLLECTIONWIDGETMIXED_H_
#define CXVIEWCOLLECTIONWIDGETMIXED_H_

#include "cxResourceVisualizationExport.h"

#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewCache.h"
#include "cxViewWidget.h"
#include "cxViewCollectionWidget.h"


class QGridLayout;

namespace cx
{

typedef boost::shared_ptr<class MultiViewCache> MultiViewCachePtr;
class ViewCollectionWidgetUsingViewContainer;
class LayoutWidgetUsingViewWidgets;

/**
 * Widget for displaying Views, minimizing number of renderwindows but keeping
 * the 3D views in separate renderwindows.
 *
 * The rationale behind this class is:
 *  - The cost of rendering several vtkRenderWindows is high, especially on new
 *    Linux NVidia cards/drivers. I.e. try to reduce the number using
 *    LayoutWidgetUsingViewCollection.
 *  - vtkRenderWindowInteractor is connected to a vtkRenderWindow, and this is
 *    used in 3D views, thus we need unique vtkRenderWindows for each 3D View.
 *
 * The solution here is to use a LayoutWidgetUsingViewCollection as basis, then
 * adding LayoutWidgetUsingViewWidgets each containing one 3D view, and placing
 * these on top of the basis in the QGridLayout.
 *
 *
 * \date 2014-09-26
 * \author Christian Askeland
 * \ingroup cx_resource_view_internal
 */
class	cxResourceVisualization_EXPORT ViewCollectionWidgetMixed : public ViewCollectionWidget
{
	Q_OBJECT
public:
	ViewCollectionWidgetMixed(RenderWindowFactoryPtr factory, QWidget *parent);
    virtual ~ViewCollectionWidgetMixed();

	ViewPtr addView(View::Type type, LayoutRegion region);
	virtual void setOffScreenRenderingAndClear(bool on);
	virtual bool getOffScreenRendering() const;
	virtual void clearViews();
	virtual void setModified();
	virtual void render();
	virtual void setGridSpacing(int val);
	virtual void setGridMargin(int val);
    virtual int getGridSpacing() const;
    virtual int getGridMargin() const;
    virtual std::vector<ViewPtr> getViews();
    virtual QPoint getPosition(ViewPtr view);
	virtual void enableContextMenuForViews(bool enable);

protected:
	ViewCollectionWidget* mBaseLayout;

private:
	void addWidgetToLayout(QGridLayout* layout, QWidget* widget, LayoutRegion region);
	void initBaseLayout();
	LayoutRegion mBaseRegion;
	LayoutRegion mTotalRegion;
	QGridLayout* mLayout;
	std::vector<ViewWidget*> mOverlays;

	MultiViewCachePtr mViewCache;
};



} // namespace cx

#endif // CXVIEWCOLLECTIONWIDGETMIXED_H_
