/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
