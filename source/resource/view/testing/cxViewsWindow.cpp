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

