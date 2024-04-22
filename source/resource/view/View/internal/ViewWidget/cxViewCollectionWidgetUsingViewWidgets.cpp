/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewCollectionWidgetUsingViewWidgets.h"
#include "cxGLHelpers.h"
#include "cxViewUtilities.h"
#include "cxLogger.h"
#include "vtkRenderWindow.h"
#include "cxMultiViewCache.h"
#include "cxSettings.h"

#include <QVBoxLayout>

namespace cx
{

//TODO: Decide which one to use: ViewWidgetWithSlider or ViewAndSlider
ViewWidgetWithSlider::ViewWidgetWithSlider(ViewWidget* view) :
    mSliceWidget(view)
{
    mSliceWidgetWithSlider = new QWidget();
    mSliceSlider = new ctkDoubleSlider(Qt::Horizontal, mSliceWidgetWithSlider);
    QVBoxLayout *widgetWithSliderLayout = new QVBoxLayout(mSliceWidgetWithSlider);
    widgetWithSliderLayout->setContentsMargins(0,0,0,0);
    widgetWithSliderLayout->addWidget(mSliceWidget);
    widgetWithSliderLayout->addWidget(mSliceSlider);
}


QWidget *ViewWidgetWithSlider::getViewWidgetWithSlider()
{
    return mSliceWidgetWithSlider;
}


QWidget *ViewWidgetWithSlider::getUsedWidget(View::Type type, bool useSlider)
{
    if(type == View::VIEW_2D && useSlider) {
        return mSliceWidgetWithSlider;
    } else {
        return mSliceWidget;
    }
}

ViewWidget *ViewWidgetWithSlider::getViewWidget()
{
    return mSliceWidget;
}


LayoutWidgetUsingViewWidgets::LayoutWidgetUsingViewWidgets(QWidget* parent) :
	ViewCollectionWidget(parent)
{
	mViewCache = MultiViewCache::create();
	mOffScreenRendering = false;

	mLayout = new QGridLayout;

	mLayout->setSpacing(2);
	mLayout->setMargin(4);

	this->setLayout(mLayout);
}

LayoutWidgetUsingViewWidgets::~LayoutWidgetUsingViewWidgets()
{
}

ViewPtr LayoutWidgetUsingViewWidgets::addView(LayoutViewData viewData)
{
	LayoutRegion region = viewData.mRegion;
	ViewAndSlider viewAndSlider = getViewAndSlider(viewData, mOffScreenRendering);

	mLayout->addWidget(viewAndSlider.getUsedWidget(), region.pos.row, region.pos.col, region.span.row, region.span.col);
	view_utils::setStretchFactors(mLayout, region, 1);

	mSliderViews.push_back(viewAndSlider);
	return viewAndSlider.mViewWidget->getView();
}

ViewAndSlider LayoutWidgetUsingViewWidgets::getViewAndSlider(LayoutViewData viewData, bool offScreenRendering)
{
	View::Type type = viewData.mType;
	ViewWidget* view = mViewCache->retrieveView(this, type, offScreenRendering);
	ViewAndSlider viewAndSlider(view);

	view->getView()->setType(type);
	view->show();

	if(useSlider(viewData.mPlane))
		viewAndSlider.getSliderWidget();//Create slider widget
	return viewAndSlider;
}

bool LayoutWidgetUsingViewWidgets::useSlider(PLANE_TYPE planeType)
{
	bool useSliderWidget = (settings()->value("View2D/useAxialSlider").toBool() && planeType == ptAXIAL)
			|| (settings()->value("View2D/useCoronalSlider").toBool() && planeType == ptCORONAL)
			|| (settings()->value("View2D/useSagittalSlider").toBool() && planeType == ptSAGITTAL);
	return useSliderWidget;
}

ctkDoubleSlider *LayoutWidgetUsingViewWidgets::getSlider(ViewPtr view)
{
	for (unsigned i=0; i<mSliderViews.size(); ++i)
	{
		ViewWidget* current = mSliderViews[i].mViewWidget;
		if (current->getView()==view)
			return mSliderViews[i].mSlider;
	}
	return nullptr;
}

void LayoutWidgetUsingViewWidgets::setOffScreenRenderingAndClear(bool on)
{
	this->clearViews();
	mOffScreenRendering = on;
}

bool LayoutWidgetUsingViewWidgets::getOffScreenRendering() const
{
	return mOffScreenRendering;
}

void LayoutWidgetUsingViewWidgets::clearViews()
{
	mViewCache->clearViews();

	for (unsigned i=0; i<mSliderViews.size(); ++i)
	{
		mSliderViews[i].getUsedWidget()->hide();
		mLayout->removeWidget(mSliderViews[i].getUsedWidget());
	}
	mSliderViews.clear();

	view_utils::setStretchFactors(mLayout, LayoutRegion(0, 0, LayoutData::MaxGridSize, LayoutData::MaxGridSize), 0);
}

void LayoutWidgetUsingViewWidgets::setModified()
{
	for (unsigned i=0; i<mSliderViews.size(); ++i)
	{
		ViewWidget* current = mSliderViews[i].mViewWidget;
		current->setModified();
	}
}

void LayoutWidgetUsingViewWidgets::render()
{
	for (unsigned i=0; i<mSliderViews.size(); ++i)
	{
		ViewWidget* current = mSliderViews[i].mViewWidget;
		current->render(); // render only changed scenegraph (shaky but smooth)
	}

    emit rendered();
}

QPoint LayoutWidgetUsingViewWidgets::getPosition(ViewPtr view)
{
    ViewWidget* widget = this->WidgetFromView(view);
    if (!widget)
    {
        CX_LOG_ERROR() << "Did not find view in layout " << view->getUid();
        return QPoint(0,0);
    }

    QPoint p = widget->mapToGlobal(QPoint(0,0));
    p = this->mapFromGlobal(p);
	return p;
}

void LayoutWidgetUsingViewWidgets::enableContextMenuForViews(bool enable)
{
	Qt::ContextMenuPolicy policy = enable ? Qt::CustomContextMenu : Qt::PreventContextMenu;
	for (unsigned i=0; i<mSliderViews.size(); ++i)
	{
		mSliderViews[i].mViewWidget->setContextMenuPolicy(policy);
	}
}

ViewWidget* LayoutWidgetUsingViewWidgets::WidgetFromView(ViewPtr view)
{
	for (unsigned i=0; i<mSliderViews.size(); ++i)
	{
		ViewWidget* current = mSliderViews[i].mViewWidget;
		if (current->getView()==view)
			return current;
	}
    return NULL;
}

void LayoutWidgetUsingViewWidgets::setGridSpacing(int val)
{
	mLayout->setSpacing(val);
}

void LayoutWidgetUsingViewWidgets::setGridMargin(int val)
{
	mLayout->setMargin(val);
}

int LayoutWidgetUsingViewWidgets::getGridSpacing() const
{
    return mLayout->spacing();
}

int LayoutWidgetUsingViewWidgets::getGridMargin() const
{
    return mLayout->margin();
}

std::vector<ViewPtr> LayoutWidgetUsingViewWidgets::getViews()
{
	std::vector<ViewPtr> retval;
	for (unsigned i=0; i<mSliderViews.size(); ++i)
		retval.push_back(mSliderViews[i].mViewWidget->getView());
	return retval;
}

} // cx
