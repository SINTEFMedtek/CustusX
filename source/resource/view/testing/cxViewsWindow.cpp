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
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>

#include "cxRenderWindowFactory.h"


namespace cxtest
{

ViewsWindow::ViewsWindow(cx::RenderWindowFactoryPtr factory)
{
	mZoomFactor = 1;

	this->setNiceSize();

	mLayoutWidget = cx::ViewCollectionWidget::createOptimizedLayout(factory);
//	mLayoutWidget = cx::ViewCollectionWidget::createViewWidgetLayout();
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
	QDesktopWidget* desktop = dynamic_cast<QApplication*>(QApplication::instance())->desktop();
	 QScreen* screen__ = qApp->screens()[0];
	//	 std::cout << "dpr " << screen__->devicePixelRatio() << std::endl;
	QRect screen = desktop->screenGeometry(desktop->primaryScreen());
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
	cx::ViewPtr view = mLayoutWidget->addView(type, cx::LayoutRegion(r,c));
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

