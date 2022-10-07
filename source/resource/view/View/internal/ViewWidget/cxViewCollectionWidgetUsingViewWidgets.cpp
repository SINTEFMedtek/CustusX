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

ViewPtr LayoutWidgetUsingViewWidgets::addView(LayoutViewData viewData)
{
	View::Type type = viewData.mType;
	LayoutRegion region = viewData.mRegion;
	ViewWidget* view = mViewCache->retrieveView(this, type, mOffScreenRendering);
	ViewAndSlider viewAndSlider(view);

	view->getView()->setType(type);
	view->setParent(this->parentWidget());

	//Only add slider to axial view for now
	//TODO: Connect slider to data
	if(viewData.mPlane == ptAXIAL)//TODO: Add option to turn slider on/off
	{
		mLayout->addWidget(viewAndSlider.getSliderWidget(), region.pos.row, region.pos.col, region.span.row, region.span.col);
	}
	else
		mLayout->addWidget(view, region.pos.row, region.pos.col, region.span.row, region.span.col);
	view_utils::setStretchFactors(mLayout, region, 1);
//    viewSliderWidget->show();
    view->show();

	mSliderViews.push_back(viewAndSlider);
	return view->getView();
}

QSlider* LayoutWidgetUsingViewWidgets::getSlider(ViewPtr view)
{
	for (unsigned i=0; i<mSliderViews.size(); ++i)
	{
		ViewWidget* current = mSliderViews[i].mViewWidget;
		if (current->getView()==view)
			return mSliderViews[i].mSlider;
	}
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
