/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewsWindow.h"

#include <QtWidgets>


#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "cxBoundingBox3D.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QGuiApplication>
#include <QScreen>

namespace cxtest
{

ViewsWindow::ViewsWindow()
{
	mZoomFactor = 1;

	this->setNiceSize();

	mLayoutWidget = cx::ViewCollectionWidget::createOptimizedLayout(this);
//	mLayoutWidget = cx::ViewCollectionWidget::createViewWidgetLayout(this);
	this->setCentralWidget(mLayoutWidget);

	mRenderingTimer = new QTimer(this);
	mRenderingTimer->start(33);
	connect(mRenderingTimer, SIGNAL(timeout()), this, SLOT(updateRender()));
}

void ViewsWindow::clearLayoutWidget()
{
	mLayoutWidget->clearViews();
}

void ViewsWindow::setNiceSize()
{
	QRect screen = QGuiApplication::primaryScreen()->geometry();
	//	std::cout << "screen: w=" << screen.width() << ", h=" << screen.height() << std::endl;
	screen.adjust(screen.width()*0.15, screen.height()*0.15, -screen.width()*0.15, -screen.height()*0.15);
	this->setGeometry(screen);
}

void ViewsWindow::setDescription(const QString& desc)
{
	this->setWindowTitle(desc);
}

ViewsWindow::~ViewsWindow()
{
	mRenderingTimer->stop();
}

cx::ViewPtr ViewsWindow::addView(cx::View::Type type, int r, int c)
{
	cx::LayoutViewData viewData;
	viewData.mType = type;
	viewData.mRegion = cx::LayoutRegion(r,c);
	cx::ViewPtr view = mLayoutWidget->addView(viewData);
	mViews.push_back(view);
	return view;
}

cx::ViewPtr ViewsWindow::add2DView(int r, int c)
{
	cx::ViewPtr view = this->addView(cx::View::VIEW_2D, r,c);

	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	view->getRenderWindow()->GetInteractor()->Disable();
	view->setZoomFactor(mZoomFactor);

	return view;
}

cx::ViewPtr ViewsWindow::getView(int index)
{
	return mViews[index];
}

bool ViewsWindow::quickRunWidget()
{
	this->show();
	this->updateRender();
	return true;
}

void ViewsWindow::updateRender()
{
	for (unsigned i=0; i<mViews.size(); ++i)
		this->prettyZoom(mViews[i]);

	mLayoutWidget->render();
}

void ViewsWindow::prettyZoom(cx::ViewPtr view)
{
	if (view->getZoomFactor()<0)
	{
		view->getRenderer()->ResetCamera();
		return;
	}

	cx::DoubleBoundingBox3D bb_s  = view->getViewport_s();
	double viewportHeightmm = bb_s.range()[1];//viewPortHeightPix*mmPerPix(view);
	double parallelscale = viewportHeightmm/2/view->getZoomFactor();

	vtkCamera* camera = view->getRenderer()->GetActiveCamera();
	camera->SetParallelScale(parallelscale);
}

} // namespace cxtest

