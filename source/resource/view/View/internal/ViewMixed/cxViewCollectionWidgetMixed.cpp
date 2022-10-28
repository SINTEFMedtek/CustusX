/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewCollectionWidgetMixed.h"
#include <QGridLayout>
#include "cxViewCollectionWidgetUsingViewContainer.h"
#include "cxViewUtilities.h"
#include "vtkRenderWindow.h"
#include "cxGLHelpers.h"
#include "cxLogger.h"
#include "cxMultiViewCache.h"

namespace cx
{

ViewCollectionWidgetMixed::ViewCollectionWidgetMixed(QWidget* parent) :
	LayoutWidgetUsingViewWidgets(parent)
{
	mLayout = new QGridLayout(this);
	this->setLayout(mLayout);
	mViewCache = MultiViewCache::create();

	mBaseLayout = new ViewCollectionWidgetUsingViewContainer(this);
	this->initBaseLayout();
	ViewCollectionWidgetMixed::setGridMargin(4);
	ViewCollectionWidgetMixed::setGridSpacing(2);
}

ViewCollectionWidgetMixed::~ViewCollectionWidgetMixed()
{
}

void ViewCollectionWidgetMixed::initBaseLayout()
{
	this->addWidgetToLayout(mLayout, mBaseLayout, LayoutRegion(0,0));
	mBaseRegion = LayoutRegion(-1,-1);
	mTotalRegion = LayoutRegion(-1,-1);
}

ViewPtr ViewCollectionWidgetMixed::addView(LayoutViewData viewData)
{
	LayoutRegion region = viewData.mRegion;
	ViewPtr view;
	mTotalRegion = merge(region, mTotalRegion);

	if (viewData.mType==View::VIEW_3D || useSlider(viewData.mPlane))
	{
		//Using cached 3D view don't work if mBaseRegion covers the 3D view region (In some cases.) CX-63
		this->mViewCache->clearCache();
		ViewAndSlider viewAndSlider = getViewAndSlider(viewData, mBaseLayout->getOffScreenRendering());
		mSliderViews.push_back(viewAndSlider);
		view = viewAndSlider.mViewWidget->getView();
		this->addWidgetToLayout(mLayout, viewAndSlider.getUsedWidget(), region);
	}
	else
	{
		mBaseRegion = merge(region, mBaseRegion);
		view = mBaseLayout->addView(viewData);

		// re-add the base widget with updated position in grid
		this->addWidgetToLayout(mLayout, mBaseLayout, mBaseRegion);
	}
	view_utils::setStretchFactors(mLayout, mTotalRegion, 1);

	return view;
}

void ViewCollectionWidgetMixed::setOffScreenRenderingAndClear(bool on)
{
	this->clearViews();
	mBaseLayout->setOffScreenRenderingAndClear(on);
}

bool ViewCollectionWidgetMixed::getOffScreenRendering() const
{
	return mBaseLayout->getOffScreenRendering();
}

void ViewCollectionWidgetMixed::addWidgetToLayout(QGridLayout* layout, QWidget* widget, LayoutRegion region)
{
	layout->addWidget(widget,
					  region.pos.row, region.pos.col,
					  region.span.row, region.span.col);
}

void ViewCollectionWidgetMixed::clearViews()
{
	LayoutWidgetUsingViewWidgets::clearViews();

	mBaseLayout->clearViews();
	mLayout->removeWidget(mBaseLayout);

	// rebuild to default state:
	view_utils::setStretchFactors(mLayout, mTotalRegion, 0);
	this->initBaseLayout();
}

void ViewCollectionWidgetMixed::setModified()
{
	LayoutWidgetUsingViewWidgets::setModified();
	mBaseLayout->setModified();
}

void ViewCollectionWidgetMixed::render()
{
	mBaseLayout->render();
	LayoutWidgetUsingViewWidgets::render();
}

void ViewCollectionWidgetMixed::setGridSpacing(int val)
{
	mLayout->setSpacing(val);
	mBaseLayout->setGridSpacing(val);
}

void ViewCollectionWidgetMixed::setGridMargin(int val)
{
	mBaseLayout->setGridMargin(0);
	mLayout->setMargin(val);
}

int ViewCollectionWidgetMixed::getGridSpacing() const
{
    return mLayout->spacing();
}

int ViewCollectionWidgetMixed::getGridMargin() const
{
    return mLayout->margin();
}

std::vector<ViewPtr> ViewCollectionWidgetMixed::getViews()
{
	std::vector<ViewPtr> retval = mBaseLayout->getViews();
	for (unsigned i=0; i<mSliderViews.size(); ++i)
		retval.push_back(mSliderViews[i].mViewWidget->getView());
	return retval;
}

QPoint ViewCollectionWidgetMixed::getPosition(ViewPtr view)
{
	for (unsigned i=0; i<mSliderViews.size(); ++i)
    {
		if (mSliderViews[i].mViewWidget->getView()==view)
        {
			QPoint p = mSliderViews[i].mViewWidget->mapToGlobal(QPoint(0,0));
            p = this->mapFromGlobal(p);
            return p;
        }
    }

    QPoint p = mBaseLayout->getPosition(view);
    p = mBaseLayout->mapToGlobal(p);
    p = this->mapFromGlobal(p);
	return p;
}

void ViewCollectionWidgetMixed::enableContextMenuForViews(bool enable)
{
	mBaseLayout->enableContextMenuForViews(enable);
	LayoutWidgetUsingViewWidgets::enableContextMenuForViews(enable);
}

} /* namespace cx */
