/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewContainerItem.h"

#include <QApplication>
#include <QDesktopWidget>
#include "vtkRenderer.h"
#include "cxBoundingBox3D.h"
#include "cxViewLinkingViewContainerItem.h"

namespace cx
{

ViewItem::ViewItem(QString uid, QString name, QWidget *parent, vtkRenderWindowPtr renderWindow, QRect rect) :
	QObject(parent),
	mGeometry(rect),
	mParent(parent)
{
	mZoomFactor = -1.0;
	mView = ViewLinkingViewContainerItem::create(this, renderWindow);
	mView->clear();
}

ViewItem::~ViewItem()
{
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
