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

#include "cxViewContainer.h"

//#include "sscViewContainer.h"
#include <QtGui>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include "cxVector3D.h"
#include "vtkRenderWindow.h"
//#include "sscViewRenderWindow.h"
#include "vtkRenderer.h"
#ifdef check
#undef check
#endif

#include "cxRep.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include <QGridLayout>


namespace cx
{

ViewContainer::ViewContainer(QWidget *parent, Qt::WindowFlags f) :
	QVTKWidget(parent, f),
	mMouseEventTarget(NULL)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
	mMTimeHash = 0;
	mMouseEventTarget = NULL;
//	mRenderWindow = vtkRenderWindowPtr::New();
	this->setLayout(new QGridLayout);
//	this->SetRenderWindow(mRenderWindow);
	std::cout << "create ViewContainer::ViewContainer with rw=" << mRenderWindow.GetPointer() << std::endl;
}

ViewContainer::~ViewContainer()
{
}

/**
  * Clears view container, deleting all layout objects
  */
void ViewContainer::clear()
{
	QLayoutItem *item;
	while ((item = getGridLayout()->takeAt(0)) != 0)
	{
		ViewItem* viewItem = dynamic_cast<ViewItem*>(item);
		delete viewItem;
//		dynamic_cast<ViewItem*>(item)->removeReps();

//		mRenderWindow->RemoveRenderer(dynamic_cast<ViewItem*>(item)->getRenderer());
//		delete dynamic_cast<ViewItem*>(item);
	}
	this->clearBackground();
	mMouseEventTarget = NULL;
}

void ViewContainer::clearBackground()
{
	return; // TODO: this crashes on mac - find out why




	if (mRenderWindow) // TODO - remove this if
	{
		vtkRendererPtr renderer = vtkRendererPtr::New();
		mRenderWindow->AddRenderer(renderer);
		renderer->SetViewport(0,0,1,1);
		renderer->Clear();
		QColor background = palette().color(QPalette::Background);
		renderer->SetBackground(background.redF(), background.greenF(), background.blueF());
		renderer->Render();
		mRenderWindow->RemoveRenderer(renderer);
	}
}

/**
  * Return this widget's grid layout object
  */
QGridLayout* ViewContainer::getGridLayout()
{
	return (QGridLayout*) layout();
}

void ViewContainer::paintEvent(QPaintEvent* event)
{
	inherited_widget::paintEvent(event);
	this->forcedUpdate();
}

void ViewContainer::forcedUpdate()
{
	this->clearBackground();
	if (this->getGridLayout())
	{
		for (int i = 0; i < this->getGridLayout()->count(); ++i)
		{
			this->getViewItem(i)->getView()->forceUpdate();
		}
	}
	mMTimeHash = 0;
}

ViewItem* ViewContainer::getViewItem(int index)
{
	return dynamic_cast<ViewItem*>(this->getGridLayout()->itemAt(index));
}


/**
  * Creates and adds a view to this container.
  * Returns a pointer to the created view item that the container owns.
  */
ViewItem *ViewContainer::addView(QString uid, int row, int col, int rowSpan, int colSpan, QString name)
{
	if (!mRenderWindow)
	{
		mRenderWindow = vtkRenderWindowPtr::New();
		this->SetRenderWindow(mRenderWindow);
	}
	// Create a viewItem for this view
	ViewItem *item = new ViewItem(uid, name, this, mRenderWindow, QRect());
	if (getGridLayout())
	{
		getGridLayout()->addItem(item, row, col, rowSpan, colSpan);

		// already done in ViewItem constructor
//		// Create and add view renderer
//		vtkRendererPtr renderer = vtkRendererPtr::New();
//		// All views' renderer viewport will to be calculated on resize - just add for now
//		mRenderWindow->AddRenderer(renderer);
//		item->setRenderer(renderer);
	}

	return item;
}

void ViewContainer::customContextMenuRequestedSlot(const QPoint& point)
{
	QWidget* sender = dynamic_cast<QWidget*>(this->sender());
	QPoint pointGlobal = sender->mapToGlobal(point);
//	emit customContextMenuRequestedInGlobalPos(pointGlobal);

	if (mMouseEventTarget)
	{
//		QRect r = mMouseEventTarget->geometry();
//		QPoint p = pos;
		mMouseEventTarget->customContextMenuRequestedGlobalSlot(pointGlobal);
	}
}

void ViewContainer::mouseMoveEvent(QMouseEvent* event)
{
	inherited_widget::mouseMoveEvent(event);
	this->handleMouseMove(event->pos(), event->buttons());
}

void ViewContainer::handleMouseMove(const QPoint &pos, const Qt::MouseButtons &buttons)
{
	if (mMouseEventTarget)
	{
		QRect r = mMouseEventTarget->geometry();
		QPoint p = pos;
		mMouseEventTarget->mouseMoveSlot(p.x() - r.left(), p.y() - r.top(), buttons);
	}
}

void ViewContainer::mousePressEvent(QMouseEvent* event)
{
	// special case for CustusX: when context menu is opened, mousereleaseevent is never called.
	// this sets the render interactor in a zoom state after each menu call. This hack prevents
	// the mouse press event in this case.
	if ((this->contextMenuPolicy() == Qt::CustomContextMenu) && event->buttons().testFlag(Qt::RightButton))
		return;

	inherited_widget::mousePressEvent(event);
	this->handleMousePress(event->pos(), event->buttons());
}
void ViewContainer::handleMousePress(const QPoint &pos, const Qt::MouseButtons & buttons)
{
	for (int i = 0; getGridLayout() && i < getGridLayout()->count(); ++i)
	{
		ViewItem *item = this->getViewItem(i);
		QRect r = item->geometry();
		if (r.contains(pos))
		{
			mMouseEventTarget = item;
			item->mousePressSlot(pos.x() - r.left(), pos.y() - r.top(), buttons);
		}
	}
}

void ViewContainer::mouseReleaseEvent(QMouseEvent* event)
{
	inherited_widget::mouseReleaseEvent(event);
	this->handleMouseRelease(event->pos(), event->buttons());
}

void ViewContainer::handleMouseRelease(const QPoint &pos, const Qt::MouseButtons &buttons)
{
	if (mMouseEventTarget)
	{
		QRect r = mMouseEventTarget->geometry();
		QPoint p = pos;
		mMouseEventTarget->mouseReleaseSlot(p.x() - r.left(), p.y() - r.top(), buttons);
		mMouseEventTarget = NULL;
	}
}

void ViewContainer::focusInEvent(QFocusEvent* event)
{
	inherited_widget::focusInEvent(event);
}

void ViewContainer::wheelEvent(QWheelEvent* event)
{
	inherited_widget::wheelEvent(event);
	for (int i = 0; layout() && i < layout()->count(); ++i)
	{
		ViewItem *item = dynamic_cast<ViewItem*>(layout()->itemAt(i));
		QRect r = item->geometry();
		QPoint p = event->pos();
		if (r.contains(p))
		{
			item->mouseWheelSlot(p.x() - r.left(), p.y() - r.top(), event->delta(), event->orientation(), event->buttons());
		}
	}
}

void ViewContainer::showEvent(QShowEvent* event)
{
	inherited_widget::showEvent(event);
}

void ViewContainer::renderAll()
{
	// First, calculate if anything has changed
	unsigned long hash = 0;
	for (int i = 0; getGridLayout() && i < getGridLayout()->count(); ++i)
	{
		ViewItem *item = this->getViewItem(i);

		hash += item->getRenderer()->GetMTime();
		hash += this->getRenderWindow()->GetMTime();
		vtkPropCollection *props = item->getRenderer()->GetViewProps();
		props->InitTraversal();
		for (vtkProp* prop = props->GetNextProp(); prop != NULL; prop = props->GetNextProp())
		{
			vtkImageActor *imageActor = vtkImageActor::SafeDownCast(prop);
			if (imageActor && imageActor->GetInput())
			{
				hash += imageActor->GetInput()->GetMTime();
			}
			hash += prop->GetMTime();
			hash += prop->GetRedrawMTime();
		}
	}
	// Then, if anything has changed, render everything anew
	if (hash != mMTimeHash)
	{
		doRender();
		mMTimeHash = hash;
	}
}

void ViewContainer::doRender()
{
	this->getRenderWindow()->Render();
}

void ViewContainer::resizeEvent( QResizeEvent *event)
{
	inherited_widget::resizeEvent(event);
	this->clearBackground();
	this->getGridLayout()->update();
}


} /* namespace cx */
