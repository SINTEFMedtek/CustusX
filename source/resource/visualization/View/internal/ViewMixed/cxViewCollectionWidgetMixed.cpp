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
#include "cxLogger.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "cxReporter.h"
#include <QLabel>
#include "cxViewCollectionWidgetUsingViewContainer.h"
#include "cxViewCollectionWidgetUsingViewWidgets.h"
#include "cxViewUtilities.h"

namespace cx
{

ViewCollectionWidgetMixed::ViewCollectionWidgetMixed()
{
	mLayout = new QGridLayout(this);
	this->setLayout(mLayout);

	mBaseLayout = new ViewCollectionWidgetUsingViewContainer();
	this->addWidgetToLayout(mLayout, mBaseLayout, LayoutRegion(0,0));
	mBaseRegion = LayoutRegion(-1,-1);
	mTotalRegion = LayoutRegion(-1,-1);

	this->setGridMargin(4);
	this->setGridSpacing(2);
}

ViewCollectionWidgetMixed::~ViewCollectionWidgetMixed()
{
}

ViewPtr ViewCollectionWidgetMixed::addView(View::Type type, LayoutRegion region)
{
	ViewPtr view;
	mTotalRegion = merge(region, mTotalRegion);

	if (type==View::VIEW_3D)
	{
		LayoutWidgetUsingViewWidgets* overlay = new LayoutWidgetUsingViewWidgets();
		overlay->setGridMargin(0);
		overlay->setGridSpacing(0);
		mOverlays.push_back(overlay);
		mOverlayRegions.push_back(region);
		view = overlay->addView(type, LayoutRegion(0,0,1,1));
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
	mBaseLayout->clearViews();
	for (unsigned i=0; i<mOverlays.size(); ++i)
		mOverlays[i]->clearViews();
	view_utils::setStretchFactors(mLayout, mTotalRegion, 0);

	this->addWidgetToLayout(mLayout, mBaseLayout, LayoutRegion(0,0));

	for (unsigned i=0; i<mOverlays.size(); ++i)
	{
		mLayout->removeWidget(mOverlays[i]);
		delete mOverlays[i];
	}
	mOverlays.clear();

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
		mOverlays[i]->render();
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

} /* namespace cx */
