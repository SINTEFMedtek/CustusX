// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscViewContainer.h"
#include <QtGui>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include "sscVector3D.h"
#include "vtkRenderWindow.h"
#include "sscViewRenderWindow.h"
#include "vtkRenderer.h"
#ifdef check
#undef check
#endif

#include "sscRep.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"
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

namespace ssc
{

ViewContainer::ViewContainer(QWidget *parent, Qt::WFlags f) :
			     ViewQVTKWidget(parent, f),
			     mRenderWindow(ViewRenderWindowPtr::New()),
			     mMTimeHash(0),
			     mMouseEventTarget(NULL)
{
	// Create default grid layout for this object
	setLayout(new QGridLayout);
	this->SetRenderWindow(mRenderWindow);
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
	mMouseEventTarget = NULL;
	while ((item = layout()->takeAt(0)) != 0)
	{
		((ViewItem *) item)->removeReps();
		
		mRenderWindow->RemoveRenderer(((ViewItem *) item)->getRenderer());
		delete (ViewItem *)item;
	}
	clearBackground();
}

void ViewContainer::clearBackground()
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
	
/**
  * Return this widget's grid layout object
  */
QGridLayout* ViewContainer::getGridLayout()
{
	return (QGridLayout*) layout();
}

void ViewContainer::paintEvent(QPaintEvent* event)
{
	if (layout())
	{
		for (int i = 0; i < layout()->count(); ++i)
		{
			((ViewItem*) layout()->itemAt(i))->forceUpdate();
		}
	}
	widget::paintEvent(event);
	mMTimeHash = 0;
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

/**
  * Creates and adds a view to this container.
  * Returns a pointer to the created view item that the container owns.
  */
ViewItem *ViewContainer::addView(QString uid, int row, int col, int rowSpan, int colSpan, QString name)
{
	// Create a viewItem for this view
	ViewItem *item = new ViewItem(uid, name, this, mRenderWindow, QRect());
	if (getGridLayout())
	{
		getGridLayout()->addItem(item, row, col, rowSpan, colSpan);

		// Create and add view renderer
		vtkRendererPtr renderer = vtkRendererPtr::New();
		// All views' renderer viewport will to be calculated on resize - just add for now
		mRenderWindow->AddRenderer(renderer);
		item->setRenderer(renderer);
	}

	return item;
}

void ViewItem::setRenderer(vtkRendererPtr renderer)
{
	mRenderer = renderer;
	renderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
}

QRect ViewItem::screenGeometry() const
{
	return QRect(ssc::View::widget()->mapToGlobal(mGeometry.topLeft()), size());
}

void ViewItem::setGeometry(const QRect &r)
{
	mGeometry = r;
	QSize size = ssc::View::widget()->size();
	double xMin = r.left()/(double)size.width();
	double xMax = (r.right() + 1)/(double)size.width();
	double yMin = (size.height() - r.bottom() - 1) / (double)size.height();
	double yMax = (size.height() - r.top()) / (double)size.height();
	getRenderer()->SetViewport(xMin, yMin, xMax, yMax);
	setSize(r.size());
	if (mSlave)
	{
		mSlave->setGeometry(r);
	}
}

void ViewContainer::mouseMoveEvent(QMouseEvent* event)
{
	widget::mouseMoveEvent(event);
	if (mMouseEventTarget)
	{
		QRect r = mMouseEventTarget->geometry();
		QPoint p = event->pos();
		mMouseEventTarget->mouseMoveSlot(p.x() - r.left(), p.y() - r.top(), event->buttons());
	}
}

void ViewContainer::mousePressEvent(QMouseEvent* event)
{
	// special case for CustusX: when context menu is opened, mousereleaseevent is never called.
	// this sets the render interactor in a zoom state after each menu call. This hack prevents
	// the mouse press event in this case.
	if ((this->contextMenuPolicy() == Qt::CustomContextMenu) && event->buttons().testFlag(Qt::RightButton))
		return;

	widget::mousePressEvent(event);
	for (int i = 0; layout() && i < layout()->count(); ++i)
	{
		ViewItem *item = (ViewItem *)layout()->itemAt(i);
		QRect r = item->geometry();
		QPoint p = event->pos();
		if (r.contains(p))
		{
			mMouseEventTarget = item;
			item->mousePressSlot(p.x() - r.left(), p.y() - r.top(), event->buttons());
		}
	}
}

void ViewContainer::mouseReleaseEvent(QMouseEvent* event)
{
	widget::mouseReleaseEvent(event);
	if (mMouseEventTarget)
	{
		QRect r = mMouseEventTarget->geometry();
		QPoint p = event->pos();
		mMouseEventTarget->mouseReleaseSlot(p.x() - r.left(), p.y() - r.top(), event->buttons());
		mMouseEventTarget = NULL;
	}
}

void ViewContainer::focusInEvent(QFocusEvent* event)
{
	widget::focusInEvent(event);
}

void ViewContainer::wheelEvent(QWheelEvent* event)
{
	widget::wheelEvent(event);
	for (int i = 0; layout() && i < layout()->count(); ++i)
	{
		ViewItem *item = (ViewItem *)layout()->itemAt(i);
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
	widget::showEvent(event);
}

void ViewContainer::renderAll()
{
	// First, calculate if anything has changed
	unsigned long hash = 0;
	for (int i = 0; layout() && i < layout()->count(); ++i)
	{
		ViewItem *item = (ViewItem *)layout()->itemAt(i);

		hash += item->getRenderer()->GetMTime();
		hash += this->GetRenderWindow()->GetMTime();
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
		this->GetRenderWindow()->Render();
		mMTimeHash = hash;
	}
}
	
void ViewContainer::resizeEvent( QResizeEvent *event)
{
	ViewQVTKWidget::resizeEvent(event);
	clearBackground();
	getGridLayout()->update();
}

} // namespace ssc
