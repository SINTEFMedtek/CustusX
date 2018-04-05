/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewWidget.h"

#include <QResizeEvent>
#include <QApplication>
#include <QDesktopWidget>
#include "vtkRenderWindow.h"
#include "cxBoundingBox3D.h"
#include "cxViewLinkingViewWidget.h"
#include "cxTypeConversions.h"
#include "cxGLHelpers.h"
#include "cxOSXHelper.h"
#include "cxRenderWindowFactory.h"

namespace cx
{

ViewWidget::ViewWidget(RenderWindowFactoryPtr factory, const QString& uid, const QString& name, QWidget *parent, Qt::WindowFlags f) :
	inherited(parent, f)
{
	mMTimeHash = 0;
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	mZoomFactor = -1.0;
	vtkRenderWindowPtr rw = factory->getRenderWindow(uid);
	mView = ViewLinkingViewWidget::create(this, rw);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
	vtkRenderWindowPtr renderWindow = mView->getRenderWindow();
	this->SetRenderWindow(renderWindow);
	mView->getRenderWindow()->GetInteractor()->EnableRenderOff();
	mView->clear();
	disableGLHiDPI(this->winId());
}

void ViewWidget::customContextMenuRequestedSlot(const QPoint& point)
{
	QWidget* sender = dynamic_cast<QWidget*>(this->sender());
	QPoint pointGlobal = sender->mapToGlobal(point);
	emit customContextMenuRequestedInGlobalPos(pointGlobal);
}

ViewRepCollectionPtr ViewWidget::getView()
{
	return mView;
}

ViewWidget::~ViewWidget()
{
	this->getView()->clear();
}

vtkRendererPtr ViewWidget::getRenderer()
{
	return this->getView()->getRenderer();
}

void ViewWidget::render()
{
	// Render is called only when mtime is changed.
	// At least on MaxOS, this is not done automatically.
	unsigned long hash = mView->computeTotalMTime();

	if (hash != mMTimeHash)
	{
		this->getRenderWindow()->Render();
		mMTimeHash = hash;

		QString msg("During rendering of view: " + this->getView()->getName());
		report_gl_error_text(cstring_cast(msg));
	}
}

void ViewWidget::resizeEvent(QResizeEvent * event)
{
	inherited::resizeEvent(event);
	QSize size = event->size();
	vtkRenderWindowInteractor* iren = mView->getRenderWindow()->GetInteractor();
	if (iren != NULL)
		iren->UpdateSize(size.width(), size.height());
	emit resized(size);
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	inherited::mouseMoveEvent(event);
	emit mouseMove(event->x(), event->y(), event->buttons());
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	// special case for CustusX: when context menu is opened, mousereleaseevent is never called.
	// this sets the render interactor in a zoom state after each menu call. This hack prevents
	// the mouse press event in this case.
	if ((this->contextMenuPolicy() == Qt::CustomContextMenu) && event->buttons().testFlag(Qt::RightButton))
		return;

	inherited::mousePressEvent(event);
	emit mousePress(event->x(), event->y(), event->buttons());
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	inherited::mouseReleaseEvent(event);
	emit mouseRelease(event->x(), event->y(), event->buttons());
}

void ViewWidget::focusInEvent(QFocusEvent* event)
{
	inherited::focusInEvent(event);
	emit focusChange(event->gotFocus(), event->reason());
}

void ViewWidget::wheelEvent(QWheelEvent* event)
{
	inherited::wheelEvent(event);
	emit mouseWheel(event->x(), event->y(), event->delta(), event->orientation(), event->buttons());
}

void ViewWidget::showEvent(QShowEvent* event)
{
	inherited::showEvent(event);
	emit shown();
}

void ViewWidget::paintEvent(QPaintEvent* event)
{
	mView->setModified();
	inherited::paintEvent(event);
}

void ViewWidget::setZoomFactor(double factor)
{
	if (similar(factor, mZoomFactor))
	{
		return;
	}
	mZoomFactor = factor;
	emit resized(this->size());
}

double ViewWidget::getZoomFactor() const
{
	return mZoomFactor;
}

DoubleBoundingBox3D ViewWidget::getViewport_s() const
{
	return transform(this->get_vpMs().inv(), this->getViewport());
}

Transform3D ViewWidget::get_vpMs() const
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
DoubleBoundingBox3D ViewWidget::getViewport() const
{
	return DoubleBoundingBox3D(0, size().width(), 0, size().height(), 0, 0);
}

double ViewWidget::mmPerPix() const
{
	// use mean mm/pix over entire screen. DONT use the height of the widget in mm,
	// this is truncated to the nearest integer.
	QDesktopWidget* desktop = dynamic_cast<QApplication*>(QApplication::instance())->desktop();
	QWidget* screen = desktop->screen(desktop->screenNumber(this));
	double r_h = (double) screen->heightMM() / (double) screen->geometry().height();
	double r_w = (double) screen->widthMM() / (double) screen->geometry().width();
	double retval = (r_h + r_w) / 2.0;
	return retval;
}

} // namespace cx
