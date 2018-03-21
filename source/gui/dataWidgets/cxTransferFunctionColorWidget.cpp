/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTransferFunctionColorWidget.h"

#include <vtkColorTransferFunction.h>

#include <limits.h>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QMouseEvent>
#include <QColorDialog>
#include <QMenu>
#include <QTimer>
#include "cxImageTF3D.h"
#include "cxImageTFData.h"
#include "cxUtilHelpers.h"
#include "cxImageLUT2D.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxMathUtils.h"


namespace cx
{
TransferFunctionColorWidget::TransferFunctionColorWidget(ActiveDataPtr activeData, QWidget* parent) :
  BaseWidget(parent, "transfer_function_color_widget", "Color Transfer Function"),
	mCurrentClickPos(INT_MIN,INT_MIN),
  mBorder(5)
{
	this->setToolTip("Set the color part of a transfer function");
  this->setFocusPolicy(Qt::StrongFocus);
	mActiveImageProxy = ActiveImageProxy::New(activeData);
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageTransferFunctionsChangedSlot()));

  mSelectedPoint.reset();
  
  // Create actions
  mCustomColorAction = new QAction(tr("Custom color..."), this);
  mRemoveColorAction = new QAction(tr("Remove point"), this);
  
  connect(mCustomColorAction, SIGNAL(triggered(bool)), this, SLOT(setColorSlot()));
  connect(mRemoveColorAction, SIGNAL(triggered()), this, SLOT(removeColorSlot()));
}

TransferFunctionColorWidget::~TransferFunctionColorWidget()
{}

void TransferFunctionColorWidget::enterEvent(QEvent* event)
{
  this->setMouseTracking(true);
}

void TransferFunctionColorWidget::leaveEvent(QEvent* event)
{
  this->setMouseTracking(false);
}


void TransferFunctionColorWidget::setData(ImagePtr image, ImageTFDataPtr tfData)
{
  if (( mImage == image )&&( mImageTF==tfData ))
    return;

  mImage = image;
  mImageTF = tfData;
  this->update();
}
  
void TransferFunctionColorWidget::activeImageTransferFunctionsChangedSlot()
{
  this->update();
}
  
void TransferFunctionColorWidget::mousePressEvent(QMouseEvent* event)
{
  QWidget::mousePressEvent(event);
  mCurrentClickPos = event->pos();

  if(event->button() == Qt::LeftButton)
  {
	  mSelectedPoint = this->selectPoint(mCurrentClickPos);
	  this->update();
  }
}

void TransferFunctionColorWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QWidget::mouseReleaseEvent(event);
  //we no longer need these values
  //mSelectedPoint.reset();
}

void TransferFunctionColorWidget::mouseMoveEvent(QMouseEvent* event)
{
  QWidget::mouseMoveEvent(event);

  if(event->buttons() == Qt::LeftButton)
  {
    // Update current screen point for use in moveCurrentAlphaPoint
	mCurrentClickPos = event->pos();
//    this->moveCurrentPoint();
	this->moveSelectedPointTo(this->getCurrentColorPoint());
  }

  this->updateTooltip(event->pos());
}

void TransferFunctionColorWidget::updateTooltip(QPoint pos)
{
	ColorPoint selected = this->selectPoint(pos);
	if (!selected.isValid())
		selected = this->getCurrentColorPoint(pos.x());
	this->updateTooltip(selected);
	this->update();
}

void TransferFunctionColorWidget::updateTooltip(ColorPoint point)
{
	QString tip = QString("color(%1)=(%2)").arg(point.intensity).arg(color2string(point.value));
//	std::cout << "updated to " << tip << std::endl;
	this->setToolTip(tip);
	reporter()->sendVolatile(tip);
//	this->setStatusTip(tip);
}


void TransferFunctionColorWidget::keyPressEvent(QKeyEvent* event)
{
	if (mSelectedPoint.isValid())
	{
		int shift = 0;
		if (event->key()==Qt::Key_Left)
			shift = -1;
		if (event->key()==Qt::Key_Right)
			shift = +1;

		if (shift!=0)
		{
			ColorPoint newPoint = mSelectedPoint;
			newPoint.intensity += shift;
			this->moveSelectedPointTo(newPoint);
			this->updateTooltip(mSelectedPoint);
			return;
		}
	}

	QWidget::keyPressEvent(event);
}

int TransferFunctionColorWidget::imageIntensity2screenX(int intensity)
{
	// Get the screen (plot) position of this point
	int retval =
			static_cast<int>(mPlotArea.left() + mPlotArea.width() *
							 (intensity - mImage->getMin()) /
							 static_cast<double>(mImage->getRange()));
	return retval;
}

int TransferFunctionColorWidget::screenX2imageIntensity(int screenX)
{
	double i = mImage->getMin() + mImage->getRange() * double(screenX - mPlotArea.left()) /(mPlotArea.width()-1);
	int retval = roundAwayFromZero(i);
	return retval;
}

void TransferFunctionColorWidget::paintEvent(QPaintEvent* event)
{
  // Don't do anything before we have an image
  if (!mImage)
    return;
  QWidget::paintEvent(event);

  QPainter painter(this);

  // Fill with white background color and grey plot area background color
  painter.fillRect(this->mFullArea, QColor(170, 170, 170));
  painter.fillRect(this->mPlotArea, QColor(200, 200, 200));

  this->paintColorBar(painter);
  this->paintColorPointsAndGenerateCache(painter);
}

void TransferFunctionColorWidget::paintColorBar(QPainter& painter)
{
	// Draw color-background

	// Use vtkColorTransferFunction for interpolation
	vtkColorTransferFunctionPtr trFunc = mImageTF->generateColorTF();
	int imin = mImageTF->getLevel() - mImageTF->getWindow()/2;
	int imax = mImageTF->getLevel() + mImageTF->getWindow()/2;

	for (int x = this->mPlotArea.left(); x <= this->mPlotArea.right(); ++x)
	{
	  int intensity = screenX2imageIntensity(x);
	  double* rgb = trFunc->GetColor(intensity);
	  painter.setPen(QColor::fromRgbF(rgb[0], rgb[1], rgb[2]));

	  if (( imin <= intensity )&&( intensity <= imax ))
	  {
		  painter.drawLine(x, mPlotArea.top(), x, mPlotArea.bottom());
	  }
	  else
	  {
		  int areaHeight = mPlotArea.bottom() - mPlotArea.top();
		  int halfAreaTop = mPlotArea.top() + areaHeight / 4;
		  int halfAreaBottom = mPlotArea.bottom() - areaHeight / 4;
		  painter.drawLine(x, halfAreaTop, x, halfAreaBottom);
	  }
	}
}

void TransferFunctionColorWidget::paintColorPointsAndGenerateCache(QPainter& painter)
{
	// Go through each point and draw squares
	ColorMap colorMap = mImageTF->getColorMap();
	this->mPointRects.clear();
	for (ColorMap::iterator colorPoint = colorMap.begin(); colorPoint != colorMap.end(); ++colorPoint)
	{
		// Get the screen (plot) position of this point
		int screenX = this->imageIntensity2screenX(colorPoint->first);
		// Draw the rectangle
		QRect pointRect(screenX - mBorder, mFullArea.top(),
						mBorder*2, mFullArea.height());

		if (colorPoint->first == mSelectedPoint.intensity)
		{
			QPen pen;
			pen.setWidth(2);
			painter.setPen(pen);
		}
		else
		{
			QPen pen;
			painter.setPen(pen);
		}

		painter.drawRect(pointRect);
		this->mPointRects[colorPoint->first] = pointRect;
	}
}

void TransferFunctionColorWidget::resizeEvent(QResizeEvent* evt)
{
  QWidget::resizeEvent(evt);

  // Calculate areas
  this->mFullArea = QRect(0, 0, width(), height());
  this->mPlotArea = QRect(mBorder, mBorder, 
                          width() - mBorder*2, height() - mBorder*2);
}

TransferFunctionColorWidget::ColorPoint TransferFunctionColorWidget::selectPoint(QPoint pos) const
{
	std::map<int, QRect>::const_iterator it = mPointRects.begin();
	for(;it != mPointRects.end(); ++it)
	{
		if (it->second.contains(pos))
		{
			ColorPoint retval;
			retval.intensity = it->first;
			ColorMap colorMap = mImageTF->getColorMap();
			if (colorMap.find(retval.intensity) != colorMap.end())
				retval.value = colorMap.find(retval.intensity)->second;
			return retval;
		}
	}
	return ColorPoint();
}

bool TransferFunctionColorWidget::isEndpoint(int intensity) const
{
	if (mPointRects.begin()->first == intensity)
		return true;
	if (mPointRects.rbegin()->first == intensity)
		return true;
	return false;
}

void TransferFunctionColorWidget::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu;
  menu.addAction(mCustomColorAction);

  mSelectedPoint = this->selectPoint(mCurrentClickPos);

  if (mSelectedPoint.isValid() && !this->isEndpoint(mSelectedPoint.intensity))
  {
    menu.addSeparator();
    menu.addAction(mRemoveColorAction);
  }
  this->update();

  menu.exec(event->globalPos());
}
  
TransferFunctionColorWidget::ColorPoint TransferFunctionColorWidget::getCurrentColorPoint()
{
	return this->getCurrentColorPoint(mCurrentClickPos.x());
}

TransferFunctionColorWidget::ColorPoint TransferFunctionColorWidget::getCurrentColorPoint(int clickX)
{
  ColorPoint point;
  if(!mImage)
	  return point;
  point.intensity = screenX2imageIntensity(clickX);
  point.intensity = constrainValue(point.intensity, mImage->getMin(), mImage->getMax());

  vtkColorTransferFunctionPtr trFunc = mImageTF->generateColorTF();

  double* rgb = trFunc->GetColor(point.intensity);
  point.value = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
  return point;
}

//void TransferFunctionColorWidget::moveCurrentPoint()
//{
//	if(!mSelectedPoint.isValid())
//		return;

//	std::pair<int,int> range = this->findAllowedMoveRangeAroundColorPoint(mSelectedPoint.intensity);

//	ColorPoint newColorPoint = this->getCurrentColorPoint();
//	newColorPoint.value = mSelectedPoint.value;
//	newColorPoint.intensity = constrainValue(newColorPoint.intensity, range.first, range.second);

//	mImageTF->moveColorPoint(mSelectedPoint.intensity, newColorPoint.intensity, newColorPoint.value);
//	mSelectedPoint = newColorPoint;
//	this->update();
//}
//void TransferFunctionColorWidget::moveCurrentPoint()
//{
//	this->moveCurrentPoint(this->getCurrentColorPoint());
//}

void TransferFunctionColorWidget::moveSelectedPointTo(ColorPoint newPoint)
{
	if(!mSelectedPoint.isValid())
		return;

	std::pair<int,int> range = this->findAllowedMoveRangeAroundColorPoint(mSelectedPoint.intensity);

//	ColorPoint newColorPoint = this->getCurrentColorPoint();
	newPoint.value = mSelectedPoint.value;
	newPoint.intensity = constrainValue(newPoint.intensity, range.first, range.second);

	mImageTF->moveColorPoint(mSelectedPoint.intensity, newPoint.intensity, newPoint.value);
	mSelectedPoint = newPoint;
	this->update();
}

std::pair<int,int> TransferFunctionColorWidget::findAllowedMoveRangeAroundColorPoint(int selectedPointIntensity)
{
	// constrain new point intensity between the two neigbours
	ColorMap colorMap = mImageTF->getColorMap();
	ColorMap::iterator pointIterator = colorMap.find(selectedPointIntensity);

	std::pair<int,int> range(mImage->getMin(), mImage->getMax());
	if (pointIterator!=colorMap.begin())
	{
		ColorMap::iterator prevPointIterator = pointIterator;
		--prevPointIterator;
		range.first = std::max(range.first, prevPointIterator->first + 1);
	}

	ColorMap::iterator nextPointIterator = pointIterator;
	++nextPointIterator;
	if (nextPointIterator!=colorMap.end())
	{
		range.second = std::min(range.second, nextPointIterator->first - 1);
	}

	return range;
}

void TransferFunctionColorWidget::setColorSlot()
{
//  setColorSlotDelayed(); // crashed sporadically
  QTimer::singleShot(1, this, SLOT(setColorSlotDelayed()));
}

void TransferFunctionColorWidget::setColorSlotDelayed()
{
	ColorPoint newPoint = mSelectedPoint;
	if (!newPoint.isValid())
		newPoint = getCurrentColorPoint();

	QColor result = QColorDialog::getColor( newPoint.value, this);

	if (result.isValid() && (result!=newPoint.value))
	{
		mImageTF->addColorPoint(newPoint.intensity, result);
	}
	this->update();
}

void TransferFunctionColorWidget::removeColorSlot()
{
	if(!this->mSelectedPoint.isValid())
		return;

	mImageTF->removeColorPoint(this->mSelectedPoint.intensity);

	this->update();
}

}//namespace cx
