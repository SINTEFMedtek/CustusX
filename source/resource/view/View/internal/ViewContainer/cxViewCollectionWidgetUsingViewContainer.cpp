/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

ViewCollectionWidgetUsingViewContainer::ViewCollectionWidgetUsingViewContainer(RenderWindowFactoryPtr factory, QWidget* parent) :
	ViewCollectionWidget(parent),
	mViewContainer(NULL)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->setSpacing(0);
	layout->setMargin(0);
	mViewContainer = new ViewContainer(factory, this);
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

void ViewCollectionWidgetUsingViewContainer::setOffScreenRenderingAndClear(bool on)
{
	this->clearViews();
	mViewContainer->setOffScreenRenderingAndClear(on);
}

bool ViewCollectionWidgetUsingViewContainer::getOffScreenRendering() const
{
	return mViewContainer->getOffScreenRendering();
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

void ViewCollectionWidgetUsingViewContainer::enableContextMenuForViews(bool enable)
{
	Qt::ContextMenuPolicy policy = enable ? Qt::CustomContextMenu : Qt::PreventContextMenu;
	mViewContainer->setContextMenuPolicy(policy);
}


} /* namespace cx */
