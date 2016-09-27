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

#include "cxViewCollectionWidgetUsingViewWidgets.h"
#include "cxGLHelpers.h"
#include "cxViewUtilities.h"
#include "cxLogger.h"
#include "vtkRenderWindow.h"
#include "cxMultiViewCache.h"

namespace cx
{

LayoutWidgetUsingViewWidgets::LayoutWidgetUsingViewWidgets(QWidget* parent) :
	ViewCollectionWidget(parent)
{
	mViewCache = MultiViewCache::create();
	mOffScreenRendering = true;

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
	ViewWidget* view = mViewCache->retrieveView(this, type, mOffScreenRendering);

	view->getView()->setType(type);

	mLayout->addWidget(view, region.pos.row, region.pos.col, region.span.row, region.span.col);
	view_utils::setStretchFactors(mLayout, region, 1);
	view->show();

	mViews.push_back(view);
	return view->getView();
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

	for (unsigned i=0; i<mViews.size(); ++i)
	{
		mViews[i]->hide();
		mLayout->removeWidget(mViews[i]);
	}
	mViews.clear();

	view_utils::setStretchFactors(mLayout, LayoutRegion(0, 0, 10, 10), 0);
}

void LayoutWidgetUsingViewWidgets::setModified()
{
	for (unsigned i=0; i<mViews.size(); ++i)
	{
		ViewWidget* current = mViews[i];
		current->setModified();
	}
}

void LayoutWidgetUsingViewWidgets::render()
{
	for (unsigned i=0; i<mViews.size(); ++i)
	{
		ViewWidget* current = mViews[i];
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
	for (unsigned i=0; i<mViews.size(); ++i)
	{
		mViews[i]->setContextMenuPolicy(policy);
	}
}

ViewWidget* LayoutWidgetUsingViewWidgets::WidgetFromView(ViewPtr view)
{
    for (unsigned i=0; i<mViews.size(); ++i)
    {
        ViewWidget* current = mViews[i];
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
	for (unsigned i=0; i<mViews.size(); ++i)
		retval.push_back(mViews[i]->getView());
	return retval;
}

} // cx
