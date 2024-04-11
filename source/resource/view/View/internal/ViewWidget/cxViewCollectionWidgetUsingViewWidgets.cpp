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

#include <QVBoxLayout>

namespace cx
{

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

ViewPtr LayoutWidgetUsingViewWidgets::addView(View::Type type, LayoutRegion region)
{
	ViewWidget* view = mViewCache->retrieveView(this->parentWidget(), type, mOffScreenRendering);

	view->getView()->setType(type);
	view->setParent(this->parentWidget());

    ViewWidgetWithSlider *widgetWithSlider = new ViewWidgetWithSlider(view);
    QWidget *usedWidget = widgetWithSlider->getUsedWidget(type, this->useSlider());
    mLayout->addWidget(usedWidget, region.pos.row, region.pos.col, region.span.row, region.span.col);
	view_utils::setStretchFactors(mLayout, region, 1);
//    viewSliderWidget->show();
    view->show();

    mViewsWithSlider.push_back(widgetWithSlider);
    mViews.push_back(view);
    return widgetWithSlider->getViewWidget()->getView();
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

    for (unsigned i=0; i<mViewsWithSlider.size(); ++i)
	{
        mViewsWithSlider[i]->getViewWidget()->hide();
        mLayout->removeWidget(mViewsWithSlider[i]->getViewWidget());
	}
    mViewsWithSlider.clear();

	view_utils::setStretchFactors(mLayout, LayoutRegion(0, 0, LayoutData::MaxGridSize, LayoutData::MaxGridSize), 0);
}

void LayoutWidgetUsingViewWidgets::setModified()
{
    for (unsigned i=0; i<mViewsWithSlider.size(); ++i)
	{
        ViewWidget* current = mViewsWithSlider[i]->getViewWidget();
		current->setModified();
	}
}

void LayoutWidgetUsingViewWidgets::render()
{
    for (unsigned i=0; i<mViewsWithSlider.size(); ++i)
	{
        ViewWidget* current = mViewsWithSlider[i]->getViewWidget();
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
    for (unsigned i=0; i<mViewsWithSlider.size(); ++i)
	{
        mViewsWithSlider[i]->setContextMenuPolicy(policy);
    }
	for (unsigned i=0; i<mViews.size(); ++i)
	{
		mViews[i]->setContextMenuPolicy(policy);
	}
}

ViewWidget* LayoutWidgetUsingViewWidgets::WidgetFromView(ViewPtr view)
{
    for (unsigned i=0; i<mViewsWithSlider.size(); ++i)
    {
        ViewWidget* current = mViewsWithSlider[i]->getViewWidget();
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
    for (unsigned i=0; i<mViewsWithSlider.size(); ++i)
        retval.push_back(mViewsWithSlider[i]->getViewWidget()->getView());
	return retval;
}

} // cx
