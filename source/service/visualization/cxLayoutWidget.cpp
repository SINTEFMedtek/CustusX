// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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

