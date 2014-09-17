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

#include "cxViewContainerItem.h"

#include <QApplication>
#include <QDesktopWidget>

//#include "sscViewContainer.h"
#include <QtGui>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include "cxVector3D.h"
#include "vtkRenderWindow.h"
//#include "cxViewRenderWindow.h"
#include "vtkRenderer.h"
//#ifdef check
//#undef check
//#endif

#include "cxRep.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include "cxViewLinkingViewContainerItem.h"


namespace cx
{

ViewItem::ViewItem(QString uid, QString name, QWidget *parent, vtkRenderWindowPtr renderWindow, QRect rect) :
	QObject(parent),
	mGeometry(rect),
	mParent(parent)
{
	mZoomFactor = -1.0;
	std::cout << "create ViewItem::ViewItem with rw=" << renderWindow.GetPointer() << std::endl;
	mView = ViewLinkingViewContainerItem::create(this, renderWindow);
//	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
	mView->clear();
}

ViewItem::~ViewItem()
{
	ViewRepCollectionPtr view = this->getView();
	view->removeReps();
	view->getRenderWindow()->RemoveRenderer(view->getRenderer());
}

void ViewItem::setZoomFactor(double factor)
{
	if (similar(factor, mZoomFactor))
	{
		return;
	}
	mZoomFactor = factor;
	emit resized(this->size());
}

void ViewItem::setGeometry(const QRect &r)
{
	mGeometry = r;
	QSize parentSize = mParent->size();
	double xMin = r.left()/(double)parentSize.width();
	double xMax = (r.right() + 1)/(double)parentSize.width();
	double yMin = (parentSize.height() - r.bottom() - 1) / (double)parentSize.height();
	double yMax = (parentSize.height() - r.top()) / (double)parentSize.height();
	this->getView()->getRenderer()->SetViewport(xMin, yMin, xMax, yMax);
	emit resized(r.size());
}

DoubleBoundingBox3D ViewItem::getViewport_s() const
{
	return transform(this->get_vpMs().inv(), this->getViewport());
}

Transform3D ViewItem::get_vpMs() const
{
	Vector3D center_vp = this->getViewport().center();
	double scale = mZoomFactor / this->mmPerPix(); // double zoomFactor = 0.3; // real magnification
	Transform3D S = createTransformScale(Vector3D(scale, scale, scale));
	Transform3D T = createTransformTranslate(center_vp);// center of viewport in viewport coordinates
	Transform3D M_vp_w = T * S; // first scale , then translate to center.
	return M_vp_w;
}

/**return the pixel viewport.
 */
DoubleBoundingBox3D ViewItem::getViewport() const
{
	return DoubleBoundingBox3D(0, size().width(), 0, size().height(), 0, 0);
}

double ViewItem::mmPerPix() const
{
	// use mean mm/pix over entire screen. DONT use the height of the widget in mm,
	// this is truncated to the nearest integer.
	QDesktopWidget* desktop = dynamic_cast<QApplication*>(QApplication::instance())->desktop();
	QWidget* screen = desktop->screen(desktop->screenNumber(mParent));
	double r_h = (double) screen->heightMM() / (double) screen->geometry().height();
	double r_w = (double) screen->widthMM() / (double) screen->geometry().width();
	double retval = (r_h + r_w) / 2.0;
	return retval;
}


} /* namespace cx */
