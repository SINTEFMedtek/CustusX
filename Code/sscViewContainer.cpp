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
			     mRenderWindow(ViewRenderWindowPtr::New())
{
	// Create default grid layout for this object
	this->SetRenderWindow(mRenderWindow);
	mCols = 0;
	mRows = 0;
	clear();
}

ViewContainer::~ViewContainer()
{
}

void ViewContainer::clear()
{
	for (int i = 0; i < mViews.size(); i++)
	{
		mViews.at(i)->removeReps();
		mRenderWindow->RemoveRenderer(mViews.at(i)->getRenderer());
	}
	setupViews(1, 1);
}

void ViewContainer::paintEvent(QPaintEvent* event)
{
	for (int i = 0; i < mViews.size(); i++)
	{
		mViews.at(i)->forceUpdate();
	}
	widget::paintEvent(event);
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

void ViewContainer::calcSize()
{
	double wf = 1.0 / mCols; // width fraction
	double hf = 1.0 / mRows; // height fraction
	QListIterator<ViewItem *> i(mViews);
	int c = 0;
	int r = 0;
	while (i.hasNext())
	{
		ViewItem *item = i.next();
		QSize grid(size().width() / mCols, size().height() / mRows);
		QRect rect;
		rect.setX(c * grid.width());
		rect.setY(r * grid.height());
		rect.setSize(grid);
		vtkRendererPtr renderer = item->getRenderer();
		renderer->SetViewport(wf * c, hf * r, wf * c + wf - 0.01, hf * r + hf - 0.01);
		item->setGLViewport(rect);
		c++;
		if (c >= mCols)
		{
			c = 0;
			r++;
		}
	}
}

void ViewContainer::setupViews(int cols, int rows)
{
	if (cols != mCols || rows != mRows)
	{
		QListIterator<ViewItem *> i(mViews);
		while (i.hasNext())
		{
			ViewItem *item = i.next();
			delete item;
		}
		mCols = cols;
		mRows = rows;
		mViews.clear();
		mViews.reserve(cols * rows);
		for (int i = 0; i < cols * rows; i++)
		{
			ViewItem *item = new ViewItem(this, mRenderWindow, QRect());
			vtkRendererPtr renderer = vtkRendererPtr::New();
			mRenderWindow->AddRenderer(renderer);
			item->setRenderer(renderer);
			mViews.append(item);
		}
	}
	calcSize();
}

void ViewItem::setRenderer(vtkRendererPtr renderer)
{
	mRenderer = renderer;
	renderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
}

QRect ViewItem::screenGeometry() const
{
	return QRect(ssc::View::widget()->mapToGlobal(getOrigin()), size());
}
	
ViewItem *ViewContainer::getView(int view)
{
	return mViews[view];
}

void ViewContainer::mouseMoveEvent(QMouseEvent* event)
{
	widget::mouseMoveEvent(event);
	emit mouseMoveSignal(event);
}

void ViewContainer::mousePressEvent(QMouseEvent* event)
{
	// special case for CustusX: when context menu is opened, mousereleaseevent is never called.
	// this sets the render interactor in a zoom state after each menu call. This hack prevents
	// the mouse press event in this case.
	if ((this->contextMenuPolicy() == Qt::CustomContextMenu) && event->buttons().testFlag(Qt::RightButton))
		return;

	widget::mousePressEvent(event);
	emit mousePressSignal(event);
}

void ViewContainer::mouseReleaseEvent(QMouseEvent* event)
{
	widget::mouseReleaseEvent(event);
	emit mouseReleaseSignal(event);
}

void ViewContainer::focusInEvent(QFocusEvent* event)
{
	widget::focusInEvent(event);
	emit focusInSignal(event);
}

void ViewContainer::wheelEvent(QWheelEvent* event)
{
	widget::wheelEvent(event);
	emit mouseWheelSignal(event);
}

void ViewContainer::showEvent(QShowEvent* event)
{
	widget::showEvent(event);
	emit showSignal(event);
}

void ViewContainer::resizeEvent(QResizeEvent *event)
{
	QSize grid;
	grid.setWidth(event->size().width() / mCols);
	grid.setHeight(event->size().height() / mRows);
	for (int i = 0; i < mViews.size(); i++)
	{
		mViews.at(i)->setSize(grid);
	}
	setupViews(mCols, mRows);
	emit resized(event->size());
}

} // namespace ssc
