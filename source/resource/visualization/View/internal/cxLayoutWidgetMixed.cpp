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

#include "cxLayoutWidgetMixed.h"
#include <QGridLayout>
#include "cxLogger.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "cxReporter.h"
#include <QLabel>
#include "cxViewCollectionLayout.h"
#include "cxViewWidgetLayout.h"

namespace cx
{

LayoutWidgetMixed::LayoutWidgetMixed()
{
	mLayout = new QGridLayout(this);
	this->setLayout(mLayout);

	mBaseLayout = new LayoutWidgetUsingViewCollection();
	this->addWidgetToLayout(mLayout, mBaseLayout, LayoutRegion(0,0));
	mBaseRegion = LayoutRegion(-1,-1);
	mTotalRegion = LayoutRegion(-1,-1);

	this->setGridMargin(4);
	this->setGridSpacing(2);
}

LayoutWidgetMixed::~LayoutWidgetMixed()
{
}

ViewPtr LayoutWidgetMixed::addView(View::Type type, LayoutRegion region)
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
	this->setStretchFactors(mTotalRegion, 1);

	return view;
}

void LayoutWidgetMixed::addWidgetToLayout(QGridLayout* layout, QWidget* widget, LayoutRegion region)
{
	layout->addWidget(widget,
					  region.pos.row, region.pos.col,
					  region.span.row, region.span.col);
}

void LayoutWidgetMixed::clearViews()
{
	mBaseLayout->clearViews();
	for (unsigned i=0; i<mOverlays.size(); ++i)
		mOverlays[i]->clearViews();
	this->setStretchFactors(mTotalRegion, 0);

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

void LayoutWidgetMixed::setModified()
{
	mBaseLayout->setModified();
	for (unsigned i=0; i<mOverlays.size(); ++i)
		mOverlays[i]->setModified();
}

void LayoutWidgetMixed::render()
{
	mBaseLayout->render();
	for (unsigned i=0; i<mOverlays.size(); ++i)
		mOverlays[i]->render();
}

void LayoutWidgetMixed::setGridSpacing(int val)
{
	mLayout->setSpacing(val);
	mBaseLayout->setGridSpacing(val);
}

void LayoutWidgetMixed::setGridMargin(int val)
{
	mBaseLayout->setGridMargin(0);
	mLayout->setMargin(val);
}

void LayoutWidgetMixed::setStretchFactors(LayoutRegion region, int stretchFactor)
{
	// set stretch factors for the affected cols to 1 in order to get even distribution
	for (int i = region.pos.col; i < region.pos.col + region.span.col; ++i)
	{
		mLayout->setColumnStretch(i, stretchFactor);
	}
	// set stretch factors for the affected rows to 1 in order to get even distribution
	for (int i = region.pos.row; i < region.pos.row + region.span.row; ++i)
	{
		mLayout->setRowStretch(i, stretchFactor);
	}
}

} /* namespace cx */
