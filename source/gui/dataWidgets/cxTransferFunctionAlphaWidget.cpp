/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTransferFunctionAlphaWidget.h"

#include <limits.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkImageAccumulate.h>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QBrush>
#include <QMouseEvent>
#include "cxImageTF3D.h"
#include "cxImageTFData.h"
#include "cxLogger.h"
#include "cxUtilHelpers.h"
#include "cxTypeConversions.h"
#include "vtkDataArray.h"
#include "cxReporter.h"

namespace cx
{
TransferFunctionAlphaWidget::TransferFunctionAlphaWidget(ActiveDataPtr activeData, QWidget* parent) :
  BaseWidget(parent, "transfer_function_alpha_widget", "Alpha Transfer Function"),
  mBorder(5),
  mReadOnly(false)
{
	this->setToolTip("Set the alpha part of a transfer function");
	this->setFocusPolicy(Qt::StrongFocus);

	mActiveImageProxy = ActiveImageProxy::New(activeData);
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageTransferFunctionsChangedSlot()));

  mSelectedAlphaPoint.reset();
}
TransferFunctionAlphaWidget::~TransferFunctionAlphaWidget()
{}

void TransferFunctionAlphaWidget::setData(ImagePtr image, ImageTFDataPtr tfData)
{
  if (( mImage == image )&&( mImageTF==tfData ))
    return;

  mImage = image;
  mImageTF = tfData;
  this->update();
}

void TransferFunctionAlphaWidget::setReadOnly(bool readOnly)
{
  mReadOnly = readOnly;
}

void TransferFunctionAlphaWidget::activeImageTransferFunctionsChangedSlot()
{
  this->update();
}
  
void TransferFunctionAlphaWidget::enterEvent(QEvent* event)
{
  this->setMouseTracking(true);
}

void TransferFunctionAlphaWidget::leaveEvent(QEvent* event)
{
  this->setMouseTracking(false);
}

void TransferFunctionAlphaWidget::mousePressEvent(QMouseEvent* event)
{
  if(mReadOnly)
    return;
  QWidget::mousePressEvent(event);

  if(event->button() == Qt::LeftButton)
  {
	  mSelectedAlphaPoint = this->selectPoint(event->pos());
  }
  else if(event->button() == Qt::RightButton)
  {
	  this->toggleSelectedPoint(event->pos());
  }

  this->update();
}

void TransferFunctionAlphaWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if(mReadOnly)
    return;
  QWidget::mouseReleaseEvent(event);
//  mCurrentAlphaPoint.reset();
}

void TransferFunctionAlphaWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!mImage)
    return;

  this->updateTooltip(event->pos());
  if(mReadOnly) //Only show tool tip if readOnly
    return;

  QWidget::mouseMoveEvent(event);

  if(event->buttons() == Qt::LeftButton)
  {
	this->moveCurrentAlphaPoint(this->getCurrentAlphaPoint(event->pos()));
	  this->update();
  }
}

void TransferFunctionAlphaWidget::keyPressEvent(QKeyEvent* event)
{
	if (mSelectedAlphaPoint.isValid())
	{
		int shift = 0;
		int alphaShift = 0;
		if (event->key()==Qt::Key_Left)
			shift = -1;
		if (event->key()==Qt::Key_Right)
			shift = +1;
		if (event->key()==Qt::Key_Down)
			alphaShift = -1;
		if (event->key()==Qt::Key_Up)
			alphaShift = +1;

		if ((shift!=0) || (alphaShift!=0))
		{
			AlphaPoint newPoint = mSelectedAlphaPoint;
			newPoint.position += shift;
			newPoint.value += alphaShift;
			this->moveCurrentAlphaPoint(newPoint);
			this->updateTooltip(mSelectedAlphaPoint);
			this->update();
			return;
		}
	}

	QWidget::keyPressEvent(event);
}

void TransferFunctionAlphaWidget::updateTooltip(QPoint pos)
{
	AlphaPoint selected = this->selectPoint(pos);
	if (!selected.isValid())
		selected = this->getCurrentAlphaPoint(pos);
	this->updateTooltip(selected);
	this->update();
}

void TransferFunctionAlphaWidget::updateTooltip(AlphaPoint point)
{
	QString tip = QString("alpha(%1)=%2").arg(point.position).arg(double(point.value)/255, 0, 'f', 2);
	this->setToolTip(tip);
	reporter()->sendVolatile(tip);
}


void TransferFunctionAlphaWidget::paintEvent(QPaintEvent* event)
{
  QWidget::paintEvent(event);
  QPainter painter(this);
  this->clearBackground(painter);

  if (!mImage)
	  return;

  this->paintHistogram(painter);
  this->paintOpacityGraph(painter);
}

void TransferFunctionAlphaWidget::clearBackground(QPainter& painter)
{
	// Fill with white global background color and grey plot area background color
	const QBrush frameBrush = QBrush(QColor(170, 170, 170));
	const QBrush backgroundBrush = QBrush(QColor(200, 200, 200));
	painter.fillRect(this->mFullArea, frameBrush);
	painter.fillRect(this->mPlotArea, backgroundBrush);

}

void TransferFunctionAlphaWidget::paintOpacityGraph(QPainter& painter)
{
	QPen pointPen, pointLinePen;
	pointPen.setColor(QColor(0, 0, 150));
	pointLinePen.setColor(QColor(150, 100, 100));

	// Go through each point and draw squares and lines
	IntIntMap opacityMap = mImageTF->getOpacityMap();

	QPoint lastScreenPoint;
	this->mPointRects.clear();
	for (IntIntMap::iterator opPoint = opacityMap.begin();
		 opPoint != opacityMap.end();
		 ++opPoint)
	{
	  // Get the screen (plot) position of this point
		AlphaPoint pt(opPoint->first, opPoint->second);
		QPoint screenPoint = this->alpha2screen(pt);

		// draw line from left edge to first point:
		if (opPoint==opacityMap.begin())
		{
			lastScreenPoint = QPoint(mPlotArea.left(), screenPoint.y());
		}

	  // Draw line from previous point
		painter.setPen(pointLinePen);
		painter.drawLine(lastScreenPoint, screenPoint);

	  // Draw the rectangle
	  QRect pointRect(screenPoint.x() - mBorder, screenPoint.y() - mBorder,
					  mBorder*2, mBorder*2);
	  if (opPoint->first==mSelectedAlphaPoint.position)
	  {
		  pointPen.setWidth(2);
		  painter.setPen(pointPen);
	  }
	  else
	  {
		  pointPen.setWidth(1);
		  painter.setPen(pointPen);
	  }
	  painter.drawRect(pointRect);
	  this->mPointRects[opPoint->first] = pointRect;

	  // Store the point
	  lastScreenPoint = screenPoint;
	}

	// draw a line from the last point to the right end
	QPoint screenPoint(mPlotArea.right(), lastScreenPoint.y());
	painter.setPen(pointLinePen);
	painter.drawLine(lastScreenPoint, screenPoint);

}

QPoint TransferFunctionAlphaWidget::alpha2screen(AlphaPoint pt) const
{
	QPoint screenPoint = QPoint(
				static_cast<int>(mPlotArea.left() + mPlotArea.width() *
								 (pt.position - mImage->getMin()) /
								 static_cast<double>(mImage->getRange())),
				static_cast<int>(mPlotArea.bottom() - mPlotArea.height() *
								 pt.value /
								 static_cast<double>(mImage->getMaxAlphaValue())) );
	return screenPoint;
}

void TransferFunctionAlphaWidget::paintHistogram(QPainter& painter)
{
	// Draw histogram
	// with log compression

	vtkImageAccumulatePtr histogram = mImage->getHistogram();
	int histogramSize = histogram->GetComponentExtent()[1] -
						histogram->GetComponentExtent()[0];

	painter.setPen(QColor(140, 140, 210));

	double numElementsInBinWithMostElements = log(histogram->GetOutput()->GetPointData()->GetScalars()->GetRange()[1]+1);
	double barHeightMult = (this->height() - mBorder*2) / numElementsInBinWithMostElements;

	double posMult = (this->width() - mBorder*2) / double(histogramSize);
	for (int i = mImage->getMin(); i <= mImage->getMax(); i++)
	{
		int x = int(std::lround(((i- mImage->getMin()) * posMult))); //Offset with min value
		int y = int(std::lround(log(double(static_cast<int*>(histogram->GetOutput()->GetScalarPointer(i - mImage->getMin(), 0, 0))[0]+1)) * barHeightMult));
	  if (y > 0)
	  {
		painter.drawLine(x + mBorder, height() - mBorder,
						 x + mBorder, height() - mBorder - y);
	  }
	}
}


void TransferFunctionAlphaWidget::resizeEvent(QResizeEvent* evt)
{
  QWidget::resizeEvent(evt);

  // Calculate areas
  this->mFullArea = QRect(0, 0, width(), height());
  this->mPlotArea = QRect(mBorder, mBorder, 
                          width() - mBorder*2, height() - mBorder*2);
}

TransferFunctionAlphaWidget::AlphaPoint TransferFunctionAlphaWidget::selectPoint(QPoint pos)
{
	std::map<int, QRect>::iterator it = mPointRects.begin();
	for(;it != mPointRects.end(); ++it)
	{
		if (it->second.contains(pos))
		{
			AlphaPoint retval;
			retval.position = it->first;
			IntIntMap opactiyMap = mImageTF->getOpacityMap();
			if (opactiyMap.find(retval.position) != opactiyMap.end())
				retval.value = opactiyMap.find(retval.position)->second;
			return retval;
		}
	}

	return AlphaPoint();
}

bool TransferFunctionAlphaWidget::isEndpoint(int intensity) const
{
	if (mPointRects.begin()->first == intensity)
		return true;
	if (mPointRects.rbegin()->first == intensity)
		return true;
	return false;
}

TransferFunctionAlphaWidget::AlphaPoint TransferFunctionAlphaWidget::getCurrentAlphaPoint(QPoint pos)
{
  AlphaPoint point;

  double dposition = mImage->getMin() + mImage->getRange() * double(pos.x() - mPlotArea.left()) / mPlotArea.width();
  double dvalue = mImage->getMaxAlphaValue() * double(mPlotArea.bottom() - pos.y())/mPlotArea.height();
	point.position = int(std::lround(dposition));
	point.value = int(std::lround(dvalue));

  point.position = constrainValue(point.position, mImage->getMin(), mImage->getMax());
  point.value = constrainValue(point.value, 0, mImage->getMaxAlphaValue());

  return point;
}

void TransferFunctionAlphaWidget::toggleSelectedPoint(QPoint pos)
{
  if(!mImage)
    return;
  mSelectedAlphaPoint = this->selectPoint(pos);
  if(!mSelectedAlphaPoint.isValid())
  {
    // Outside any of the rectangles
	AlphaPoint point = getCurrentAlphaPoint(pos);
    mImageTF->addAlphaPoint(point.position,point.value);
	mSelectedAlphaPoint = this->selectPoint(pos);
  }
  else if(!this->isEndpoint(mSelectedAlphaPoint.position))
  {
    // Inside one of the rectangles
	if(mSelectedAlphaPoint.isValid())
	  mImageTF->removeAlphaPoint(mSelectedAlphaPoint.position);
	mSelectedAlphaPoint.reset();
  }

  this->update();
}

void TransferFunctionAlphaWidget::moveCurrentAlphaPoint(AlphaPoint newAlphaPoint)
{
	if(!mSelectedAlphaPoint.isValid())
		return;

	newAlphaPoint.value = constrainValue(newAlphaPoint.value, 0, 255);

	std::pair<int,int> range = this->findAllowedMoveRangeAroundAlphaPoint(mSelectedAlphaPoint.position);
	newAlphaPoint.position = constrainValue(newAlphaPoint.position, range.first, range.second);

	mImageTF->moveAlphaPoint(mSelectedAlphaPoint.position, newAlphaPoint.position, newAlphaPoint.value);

	mSelectedAlphaPoint = newAlphaPoint;
	this->update();
}

std::pair<int,int> TransferFunctionAlphaWidget::findAllowedMoveRangeAroundAlphaPoint(int selectedPointIntensity)
{
	// constrain new point intensity between the two neigbours
	IntIntMap opacityMap = mImageTF->getOpacityMap();
	IntIntMap::iterator pointIterator = opacityMap.find(selectedPointIntensity);

	std::pair<int,int> range(mImage->getMin(), mImage->getMax());
	if (pointIterator!=opacityMap.begin())
	{
		IntIntMap::iterator prevPointIterator = pointIterator;
		--prevPointIterator;
		range.first = std::max(range.first, prevPointIterator->first + 1);
	}

	IntIntMap::iterator nextPointIterator = pointIterator;
	++nextPointIterator;
	if (nextPointIterator!=opacityMap.end())
	{
		range.second = std::min(range.second, nextPointIterator->first - 1);
	}

	return range;
}


}//namespace cx
