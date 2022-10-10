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

#include <QHBoxLayout>
#include <QSlider>
#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewCache.h"
#include "cxViewWidget.h"
#include "cxViewCollectionWidget.h"

#include "ctkDoubleSlider.h"

class QGridLayout;

namespace cx
{

typedef boost::shared_ptr<class MultiViewCache> MultiViewCachePtr;

struct ViewAndSlider
{
	ViewAndSlider(ViewWidget* viewWidget):
		mViewWidget(viewWidget) {}
	~ViewAndSlider()
	{
		QHBoxLayout * mSliderlayout = nullptr;
		QWidget * mSliderWidget = nullptr;
		QSlider *mSlider = nullptr;
	}
	ViewWidget* mViewWidget;
	QHBoxLayout * mSliderlayout = nullptr;
	QWidget * mSliderWidget = nullptr;
	QSlider *mSlider = nullptr;

	QWidget * getSliderWidget()
	{
		if(!mSliderWidget)
		{
			mSliderWidget = new QWidget();
			mSlider = new QSlider(Qt::Vertical, mSliderWidget);
			mSliderlayout = new QHBoxLayout(mSliderWidget);
			mSliderlayout->addWidget(mViewWidget);
			mSliderlayout->addWidget(mSlider);
		}
		return mSliderWidget;
	}

	QWidget * getUsedWidget()
	{
		if(useSlider())
			return mSliderWidget;
		return mViewWidget;
	}
	bool useSlider()
	{
		if(mSliderWidget)
			return true;
		return false;
	}
};

/**
 * ViewWidget with added Slider for manual slicing through 2D slices
 *
 * \ingroup cx_resource_view_internal
 * \date 2023-01-04
 * \author Geir Arne Tangen
 */
class cxResourceVisualization_EXPORT ViewWidgetWithSlider : public QWidget
{
    Q_OBJECT
private:
    ctkDoubleSlider *mSliceSlider;
    ViewWidget      *mSliceWidget;
    QWidget         *mSliceWidgetWithSlider;

public:
    ViewWidgetWithSlider(ViewWidget *view);
    QWidget*        getViewWidgetWithSlider();
    QWidget*        getUsedWidget(View::Type type, bool useSlider);
    ViewWidget*     getViewWidget();
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
	virtual QSlider* getSlider(ViewPtr view);

protected:
	std::vector<ViewAndSlider> mSliderViews;
	QGridLayout* mLayout; ///< the layout
	MultiViewCachePtr mViewCache;

private:
    ViewWidget* WidgetFromView(ViewPtr view);

	bool mOffScreenRendering;
};

} // namespace cx

#endif /* CXVIEWCOLLECTIONWIDGETUSINGVIEWWIDGETS_H_ */
