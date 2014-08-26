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

#include "cxLayoutWidget.h"
#include <QGridLayout>
#include "cxLogger.h"

namespace cx
{

LayoutWidget::LayoutWidget()
{
	mLayout = new QGridLayout;

	mLayout->setSpacing(2);
	mLayout->setMargin(4);
	this->setLayout(mLayout);

	mViewCache2D.reset(new ViewCache<ViewWidget>(this,	"View2D"));
	mViewCache3D.reset(new ViewCache<ViewWidget>(this, "View3D"));
	mViewCacheRT.reset(new ViewCache<ViewWidget>(this, "ViewRT"));
}

LayoutWidget::~LayoutWidget()
{
}

void LayoutWidget::setStretchFactors(LayoutRegion region, int stretchFactor)
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

void LayoutWidget::addView(ViewWidget* view, LayoutRegion region)
{
	mLayout->addWidget(view, region.pos.row, region.pos.col, region.span.row, region.span.col);
	this->setStretchFactors(region, 1);
	view->show();
	mViews.push_back(view);
}

void LayoutWidget::clearViews()
{
	mViewCache2D->clearUsedViews();
	mViewCache3D->clearUsedViews();
	mViewCacheRT->clearUsedViews();

	for (unsigned i=0; i<mViews.size(); ++i)
	{
		mViews[i]->hide();
		mLayout->removeWidget(mViews[i]);
	}
	mViews.clear();

	this->setStretchFactors(LayoutRegion(0, 0, 10, 10), 0);
}

} // namespace cx

