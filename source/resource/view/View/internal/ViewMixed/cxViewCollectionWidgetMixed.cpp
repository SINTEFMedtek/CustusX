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
#include "cxViewCollectionWidgetUsingViewWidgets.h"
#include "cxViewUtilities.h"
#include "vtkRenderWindow.h"
#include "cxGLHelpers.h"
#include "cxLogger.h"
#include "cxMultiViewCache.h"
#include "cxRenderWindowFactory.h"

namespace cx
{

ViewCollectionWidgetMixed::ViewCollectionWidgetMixed(RenderWindowFactoryPtr factory, QWidget* parent) :
	ViewCollectionWidget(parent)
{
	mLayout = new QGridLayout(this);
	this->setLayout(mLayout);
	mViewCache = MultiViewCache::create(factory);

	mBaseLayout = new ViewCollectionWidgetUsingViewContainer(factory, this);
	this->initBaseLayout();
	this->setGridMargin(4);
	this->setGridSpacing(2);
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

ViewPtr ViewCollectionWidgetMixed::addView(View::Type type, LayoutRegion region)
{
	ViewPtr view;
	mTotalRegion = merge(region, mTotalRegion);

	if (type==View::VIEW_3D)
	{
		//Using cached 3D view don't work if mBaseRegion covers the 3D view region (In some cases.) CX-63
		this->mViewCache->clearCache();
		ViewWidget* overlay = this->mViewCache->retrieveView(this, View::VIEW_3D, mBaseLayout->getOffScreenRendering());
		overlay->getView()->setType(type);
		overlay->show();
		mOverlays.push_back(overlay);
		view = overlay->getView();
		this->addWidgetToLayout(mLayout, overlay, region);
	}
	else
	{
		mBaseRegion = merge(region, mBaseRegion);
		view = mBaseLayout->addView(type, region);

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
	mViewCache->clearViews();

	for (unsigned i=0; i<mOverlays.size(); ++i)
	{
		mOverlays[i]->hide();
		mLayout->removeWidget(mOverlays[i]);
	}
	mOverlays.clear();

	mBaseLayout->clearViews();
	mLayout->removeWidget(mBaseLayout);

	// rebuild to default state:
	view_utils::setStretchFactors(mLayout, mTotalRegion, 0);
	this->initBaseLayout();
}

void ViewCollectionWidgetMixed::setModified()
{
	mBaseLayout->setModified();
	for (unsigned i=0; i<mOverlays.size(); ++i)
		mOverlays[i]->setModified();
}

void ViewCollectionWidgetMixed::render()
{
	mBaseLayout->render();

	for (unsigned i=0; i<mOverlays.size(); ++i)
	{
		mOverlays[i]->render();
	}

    emit rendered();
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
	for (unsigned i=0; i<mOverlays.size(); ++i)
		retval.push_back(mOverlays[i]->getView());
	return retval;
}

QPoint ViewCollectionWidgetMixed::getPosition(ViewPtr view)
{
    for (unsigned i=0; i<mOverlays.size(); ++i)
    {
        if (mOverlays[i]->getView()==view)
        {
            QPoint p = mOverlays[i]->mapToGlobal(QPoint(0,0));
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
	Qt::ContextMenuPolicy policy = enable ? Qt::CustomContextMenu : Qt::PreventContextMenu;
	for (unsigned i=0; i<mOverlays.size(); ++i)
	{
		mOverlays[i]->setContextMenuPolicy(policy);
	}
}

} /* namespace cx */
