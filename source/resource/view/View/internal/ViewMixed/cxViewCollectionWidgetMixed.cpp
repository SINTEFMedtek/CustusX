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

#include "cxViewCollectionWidgetMixed.h"
#include <QGridLayout>
#include "cxViewCollectionWidgetUsingViewContainer.h"
#include "cxViewCollectionWidgetUsingViewWidgets.h"
#include "cxViewUtilities.h"
#include "vtkRenderWindow.h"
#include "cxGLHelpers.h"
#include "cxLogger.h"

namespace cx
{

ViewCollectionWidgetMixed::ViewCollectionWidgetMixed(QWidget* parent) :
	ViewCollectionWidget(parent)
{
	mLayout = new QGridLayout(this);
	this->setLayout(mLayout);
	mViewCacheOverlay.reset(new ViewCache<ViewWidget>(this, "Overlay"));

	this->initBaseLayout();
}

ViewCollectionWidgetMixed::~ViewCollectionWidgetMixed()
{
}

void ViewCollectionWidgetMixed::initBaseLayout()
{
	mBaseLayout = new ViewCollectionWidgetUsingViewContainer(this);
	this->addWidgetToLayout(mLayout, mBaseLayout, LayoutRegion(0,0));
	mBaseRegion = LayoutRegion(-1,-1);
	mTotalRegion = LayoutRegion(-1,-1);

	this->setGridMargin(4);
	this->setGridSpacing(2);
}

ViewPtr ViewCollectionWidgetMixed::addView(View::Type type, LayoutRegion region)
{
	ViewPtr view;
	mTotalRegion = merge(region, mTotalRegion);

	if (type==View::VIEW_3D)
	{
		ViewWidget* overlay = this->mViewCacheOverlay->retrieveView();
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

void ViewCollectionWidgetMixed::addWidgetToLayout(QGridLayout* layout, QWidget* widget, LayoutRegion region)
{
	layout->addWidget(widget,
					  region.pos.row, region.pos.col,
					  region.span.row, region.span.col);
}

void ViewCollectionWidgetMixed::clearViews()
{
	mViewCacheOverlay->clearUsedViews();

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
	this->addWidgetToLayout(mLayout, mBaseLayout, LayoutRegion(0,0));
	mBaseRegion = LayoutRegion(-1,-1);
	mTotalRegion = LayoutRegion(-1,-1);
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

} /* namespace cx */
