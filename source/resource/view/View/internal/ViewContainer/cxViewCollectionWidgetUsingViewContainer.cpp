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

#include "cxViewCollectionWidgetUsingViewContainer.h"

#include <QGridLayout>

#include "cxViewContainerItem.h"
#include "cxViewContainer.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "cxLogger.h"

namespace cx
{

ViewCollectionWidgetUsingViewContainer::ViewCollectionWidgetUsingViewContainer(QWidget* parent) :
	ViewCollectionWidget(parent),
	mViewContainer(NULL)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->setSpacing(0);
	layout->setMargin(0);
	mViewContainer = new ViewContainer(this);
	mViewContainer->getGridLayout()->setSpacing(2);
	mViewContainer->getGridLayout()->setMargin(4);
	layout->addWidget(mViewContainer);
}

ViewCollectionWidgetUsingViewContainer::~ViewCollectionWidgetUsingViewContainer()
{
}

ViewPtr ViewCollectionWidgetUsingViewContainer::addView(View::Type type, LayoutRegion region)
{
	mViewContainer->show();
	static int nameGenerator = 0;
	QString uid = QString("view-%1-%2")
			.arg(nameGenerator++)
			.arg(reinterpret_cast<long>(this));

	ViewItem* viewItem = mViewContainer->addView(uid, region, uid);
	ViewPtr view = viewItem->getView();

	viewItem->getView()->setType(type);
	mViews.push_back(view);
	return view;
}

void ViewCollectionWidgetUsingViewContainer::clearViews()
{
	mViews.clear();
	mViewContainer->hide();
	mViewContainer->clear();
}

void ViewCollectionWidgetUsingViewContainer::setModified()
{
	mViewContainer->setModified();
}

void ViewCollectionWidgetUsingViewContainer::render()
{
	mViewContainer->renderAll();
    emit rendered();
}

void ViewCollectionWidgetUsingViewContainer::setGridSpacing(int val)
{
	mViewContainer->getGridLayout()->setSpacing(val);
}

void ViewCollectionWidgetUsingViewContainer::setGridMargin(int val)
{
	mViewContainer->getGridLayout()->setMargin(val);
}

int ViewCollectionWidgetUsingViewContainer::getGridSpacing() const
{
    return mViewContainer->getGridLayout()->spacing();
}

int ViewCollectionWidgetUsingViewContainer::getGridMargin() const
{
    return mViewContainer->getGridLayout()->margin();
}

std::vector<ViewPtr> ViewCollectionWidgetUsingViewContainer::getViews()
{
	return mViews;
}

QPoint ViewCollectionWidgetUsingViewContainer::getPosition(ViewPtr view)
{
    Eigen::Array2i size(view->getRenderWindow()->GetSize());
    Eigen::Array2i size_renderer(view->getRenderer()->GetSize());
    Eigen::Array2i p_vc(view->getRenderer()->GetOrigin());
    p_vc[1] += size_renderer[1] - 1; // upper left corner (add extent = size-1)
    p_vc[1] = size[1] - p_vc[1] - 1; // flip y axis vtk->qt

    QPoint p(p_vc[0], p_vc[1]);
    p = mViewContainer->mapToGlobal(p);
    p = this->mapFromGlobal(p);
    return p;
}


} /* namespace cx */
