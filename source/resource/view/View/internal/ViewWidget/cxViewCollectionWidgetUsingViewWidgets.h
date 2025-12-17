/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_
#define CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_

#include "cxResourceVisualizationExport.h"

#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewWidget.h"
#include "cxViewCollectionWidget.h"

class QGridLayout;
class QHBoxLayout;
class ctkDoubleSlider;

namespace cx
{

typedef boost::shared_ptr<class MultiViewCache> MultiViewCachePtr;

struct cxResourceVisualization_EXPORT ViewAndSlider
{
	ViewAndSlider(ViewWidget* viewWidget):
		mViewWidget(viewWidget) {}
	~ViewAndSlider();
	QWidget *getSliderWidget();
	QWidget *getUsedWidget();
	bool useSlider();
	ViewWidget* mViewWidget;
	QHBoxLayout * mSliderlayout = nullptr;
	QWidget * mSliderWidget = nullptr;
	ctkDoubleSlider *mSlider = nullptr;
};

/**
 * Widget for displaying Views, Containing a QGridLayout of QVTKWidgets,
 * one for each view.
 *
 * \ingroup cx_resource_view_internal
 * \date 2013-11-05
 * \author Christian Askeland
 */
class cxResourceVisualization_EXPORT LayoutWidgetUsingViewWidgets : public ViewCollectionWidget
{
	Q_OBJECT
public:
	LayoutWidgetUsingViewWidgets(QWidget *parent);
	virtual ~LayoutWidgetUsingViewWidgets();

	virtual ViewPtr addView(LayoutViewData viewData);
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
	virtual ctkDoubleSlider* getSlider(ViewPtr view);

protected:
	std::vector<ViewAndSlider> mSliderViews;
	ViewAndSlider getViewAndSlider(LayoutViewData viewData, bool offScreenRendering);
	bool useSlider(PLANE_TYPE planeType);

	QGridLayout* mLayout; ///< the layout
	MultiViewCachePtr mViewCache;
private:
	ViewWidget* WidgetFromView(ViewPtr view);

	bool mOffScreenRendering;
};

} // namespace cx

#endif /* CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_ */
