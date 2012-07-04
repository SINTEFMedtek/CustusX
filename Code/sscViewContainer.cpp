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
	setLayout(new QGridLayout);
	this->SetRenderWindow(mRenderWindow);
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

QGridLayout* ViewContainer::getLayout()
{
	return (QGridLayout*) layout();
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

void ViewContainer::setupViews(int cols, int rows)
{
	double wf = 1.0 / cols; // width fraction
	double hf = 1.0 / rows; // height fraction

	mViews.clear();
	mViews.reserve(cols * rows);

	// Clear existing layout
	QLayout *widgetLayout = layout();
	if (widgetLayout)
	{
		delete widgetLayout;
		// Create new layout
		setLayout(new QGridLayout);
	}

	QSize grid;
	grid.setWidth(size().width() / cols);
	grid.setHeight(size().height() / rows);
	for (int c = 0; c < cols; c++)
	{
		for (int r = 0; r < rows; r++)
		{
			ViewItem *item = new ViewItem(this, mRenderWindow, grid);
			vtkRendererPtr renderer = vtkRendererPtr::New();
			// Calculate the renderer's viewport
			renderer->SetViewport(wf * c, hf * r, wf * c + wf - 0.01, hf * r + hf - 0.01);
			mRenderWindow->AddRenderer(renderer);
			item->setRenderer(renderer);
			((QGridLayout*) widgetLayout)->addItem(item, r, c);
			mViews.push_back(item);
		}
	}
}

void ViewItem::setRenderer(vtkRendererPtr renderer)
{
	mRenderer = renderer;
	renderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
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
	grid.setWidth(event->size().width() / ((QGridLayout*)layout())->columnCount());
	grid.setHeight(event->size().height() / ((QGridLayout*)layout())->columnCount());
	for (int i = 0; i < mViews.size(); i++)
	{
		mViews.at(i)->setSize(grid);
	}
	emit resized(event->size());
}

} // namespace ssc
