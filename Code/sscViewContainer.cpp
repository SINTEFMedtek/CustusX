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
	while ((item = layout()->takeAt(0)) != 0)
	{
		((ViewItem *) item)->removeReps();
		mRenderWindow->RemoveRenderer(((ViewItem *) item)->getRenderer());
	}

	QLayout *viewLayout = layout();
	if (viewLayout)
	{
		delete viewLayout;
		setLayout(new QGridLayout);
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
	if (layout())
	{
		for (int i = 0; i < layout()->count(); ++i)
		{
			((ViewItem*) layout()->itemAt(i))->forceUpdate();
		}
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

/**
  * Creates and adds a view to this container.
  * Returns a pointer to the created view item that the container owns.
  */
ViewItem *ViewContainer::addView(int row, int col, int rowSpan, int colSpan)
{
	// Create a viewItem for this view
	ViewItem *item = new ViewItem(this, mRenderWindow, QRect());
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

/**
  * Resize the container content (views), based on it's size, colums, rows and viewspan
  */
void ViewContainer::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();

	if (layout())
	{
		int cols = getGridLayout()->columnCount();
		int rows = getGridLayout()->rowCount();
		double itemWidthFactor = 1.0 / cols;
		double itemHeightFactor = 1.0 / rows;

		int itemCol, itemColSpan;
		int itemRow, itemRowSpan;
		double itemX, itemY, itemWidth, itemHeight;

		for (int i = 0; i < layout()->count(); ++i)
		{
			ViewItem* item = (ViewItem*) layout()->itemAt(i);
			// Get current item's column and row position and span
			getGridLayout()->getItemPosition(i, &itemRow, &itemCol, &itemRowSpan, &itemColSpan);

			itemX = (size.width() / cols) * itemCol;
			itemY = (size.height() / rows) * itemRow;
			itemWidth = (size.width() / cols) * itemColSpan;
			itemHeight = (size.height() / rows) * itemRowSpan;

			// Calculate render viewport
			vtkRendererPtr renderer = item->getRenderer();
			double xMin = itemWidthFactor * itemCol;
			double xMax = itemWidthFactor * (itemCol + itemColSpan);
			double yMin = itemHeightFactor * (rows - (itemRow + itemRowSpan));
			double yMax = itemHeightFactor * (rows - itemRow);
			// std::cout << "xMin: " << xMin << " yMin: " << yMin << " xMax: " << xMax << " yMax: " << yMax << std::endl;
			renderer->SetViewport(xMin, yMin, xMax, yMax);

			// Set item size and geometry
			item->setSize(QSize(itemWidth, itemHeight));
			QRect itemGeometry;
			itemGeometry.setX(itemX);
			itemGeometry.setY(itemY);
			itemGeometry.setSize(item->size());
			item->setGeometry(itemGeometry);
		}
	}

	emit resized(size);
}

} // namespace ssc
