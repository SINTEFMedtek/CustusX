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


#include "cxViewWidget.h"
#include <QtWidgets>

#include <QApplication>
#include <QDesktopWidget>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include "cxVector3D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "cxLogger.h"
#ifdef check
#undef check
#endif

#include "cxRep.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include "cxViewLinkingViewWidget.h"
/* Copy/pasted from qitemdelegate.cpp
 \internal

 Note that on Mac, if /usr/include/AssertMacros.h is included prior
 to QItemDelegate, and the application is building in debug mode, the
 check(assertion) will conflict with QItemDelegate::check.

 To avoid this problem, add

 #ifdef check
 #undef check
 #endif

 after including AssertMacros.h
 */

namespace cx
{


///--------------------------------------------------------

ViewWidget::ViewWidget(QWidget *parent, Qt::WindowFlags f) :
	inherited(parent, f),
//	View(this, this->size()),
	mRenderWindow(vtkRenderWindowPtr::New())
{
	mMTimeHash = 0;
	mBackgroundColor = QColor("black");
	mUid = "";
	mName = "";
	mType = View::VIEW;

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
	this->SetRenderWindow(mRenderWindow);
	clear();
}

ViewWidget::ViewWidget(const QString& uid, const QString& name, QWidget *parent, Qt::WindowFlags f) :
	inherited(parent, f),
//	View(this, this->size(), uid, name),
	mRenderWindow(vtkRenderWindowPtr::New())
{
	mMTimeHash = 0;
	mBackgroundColor = QColor("black");
//	mParent = parent;
	mUid = uid;
	mName = name;
	mType = View::VIEW;

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
	this->SetRenderWindow(mRenderWindow);
	clear();
}

void ViewWidget::customContextMenuRequestedSlot(const QPoint& point)
{
	QWidget* sender = dynamic_cast<QWidget*>(this->sender());
	QPoint pointGlobal = sender->mapToGlobal(point);
	emit customContextMenuRequestedInGlobalPos(pointGlobal);
}

ViewPtr ViewWidget::getView()
{
	ViewPtr retval = mView.lock();
	if (!retval)
	{
		retval.reset(new ViewLinkingViewWidget(this));
		mView = retval;
	}
	return retval;
}

ViewWidget::~ViewWidget()
{
	this->clear();
}

QString ViewWidget::getTypeString() const
{
	switch (this->getType())
	{
	case View::VIEW:
		return "View";
	case View::VIEW_2D:
		return "View2D";
	case View::VIEW_3D:
		return "View3D";
	case View::VIEW_REAL_TIME:
		return "ViewRealTime";
	}
	return "";
}

QString ViewWidget::getUid()
{
	return mUid;
}

QString ViewWidget::getName()
{
	return mName;
}

vtkRendererPtr ViewWidget::getRenderer() const
{
	return mRenderer;
}

void ViewWidget::addRep(const RepPtr& rep)
{
	if (hasRep(rep))
	{
		return;
	}

	rep->connectToView(this->getView());
	mReps.push_back(rep);
}

void ViewWidget::setBackgroundColor(QColor color)
{
	mBackgroundColor = color;
	if (mRenderer)
	{
		mRenderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
	}
}

/**clear all content of the view. This ensures that props added from
 * outside the rep system also is cleared, and data not cleared with
 * RemoveAllViewProps() (added to fix problem in snw ultrasound rep,
 * data was not cleared, dont know why).
 */
void ViewWidget::clear()
{
	removeReps();

	if (mRenderer)
		mRenderWindow->RemoveRenderer(mRenderer);
	mRenderer = vtkRendererPtr::New();
	mRenderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
	mRenderWindow->AddRenderer(mRenderer);
}

void ViewWidget::removeReps()
{
	for (RepsIter it = mReps.begin(); it != mReps.end(); ++it)
	{
		(*it)->disconnectFromView(this->getView());
	}
	mReps.clear();
}

void ViewWidget::removeRep(const RepPtr& rep)
{
	RepsIter it = std::find(mReps.begin(), mReps.end(), rep);

	if (it == mReps.end())
	{
		return;
	}

	rep->disconnectFromView(this->getView());
	mReps.erase(it);
}

std::vector<RepPtr> ViewWidget::getReps()
{
	return mReps;
}

bool ViewWidget::hasRep(const RepPtr& rep) const
{
	return std::count(mReps.begin(), mReps.end(), rep);
}

void ViewWidget::resizeEvent(QResizeEvent * event)
{
	inherited::resizeEvent(event);
	QSize size = event->size();
	vtkRenderWindowInteractor* iren = mRenderWindow->GetInteractor();
	if (iren != NULL)
		iren->UpdateSize(size.width(), size.height());
	emit resized(size);
}

void ViewWidget::print(std::ostream& os)
{
	Indent ind;
	printSelf(os, ind);
}

void ViewWidget::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "mUid: " << mUid << std::endl;
	os << indent << "mName: " << mName << std::endl;
	os << indent << "NumberOfReps: " << mReps.size() << std::endl;

	for (unsigned i = 0; i < mReps.size(); ++i)
	{
		os << indent << "<Rep child " << i << ">" << std::endl;
		mReps[i]->printSelf(os, indent.stepDown());
		os << indent << "</Rep child " << i << ">" << std::endl;
	}

	if (indent.includeDetails())
	{
		os << indent << "<RenderWindow>" << std::endl;
		mRenderWindow->PrintSelf(os, indent.getVtkIndent().GetNextIndent());
		os << indent << "</RenderWindow>" << std::endl;
		os << indent << "<Renderer>" << std::endl;
		mRenderer->PrintSelf(os, indent.getVtkIndent().GetNextIndent());
		os << indent << "</Renderer>" << std::endl;
		os << indent << "<Props>" << std::endl;
		vtkPropCollection* collection = mRenderer->GetViewProps();
		collection->InitTraversal();
		vtkProp* prop = collection->GetNextProp();
		while (prop)
		{
			os << indent << indent << "<Prop>" << std::endl;
			prop->PrintSelf(os, indent.getVtkIndent().GetNextIndent().GetNextIndent());
			os << indent << indent << "</Prop>" << std::endl;
			prop = collection->GetNextProp();
		}
		os << indent << "</Props>" << std::endl;
	}
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
	mMTimeHash = 0;
	inherited::paintEvent(event);
}

void ViewWidget::render()
{
//	std::cout << "ViewWidget::render(QW) " << size().width() << "," << size().height() << std::endl;
//	std::cout << "ViewWidget::render(RW) " << this->getRenderWindow()->GetSize()[0] << "," << this->getRenderWindow()->GetSize()[1] << std::endl;
//	Eigen::Array4d vp(this->getRenderer()->GetViewport());
//	std::cout << "ViewWidget::render(VP) " << vp << std::endl;
//	this->getRenderWindow()->Render();
//	return; //HACHACKHACK

	// Render is called only when mtime is changed.
	// At least on MaxOS, this is not done automatically.
	unsigned long hash = 0;

	hash += this->getRenderer()->GetMTime();
	hash += this->getRenderWindow()->GetMTime();
	vtkPropCollection* props = this->getRenderer()->GetViewProps();
	props->InitTraversal();
	for (vtkProp* prop = props->GetNextProp(); prop != NULL; prop = props->GetNextProp())
	{
		vtkImageActor* imageActor = vtkImageActor::SafeDownCast(prop);
		if (imageActor && imageActor->GetInput())
		{
			hash += imageActor->GetInput()->GetMTime();
		}
		hash += prop->GetMTime();
		hash += prop->GetRedrawMTime();
	}
	if (hash != mMTimeHash)
	{
		this->getRenderWindow()->Render();
		mMTimeHash = hash;
	}
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
