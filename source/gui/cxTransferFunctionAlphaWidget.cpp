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
#include "sscDataManager.h"
#include "sscImageTF3D.h"
#include "sscMessageManager.h"

namespace cx
{
TransferFunctionAlphaWidget::TransferFunctionAlphaWidget(QWidget* parent) :
  QWidget(parent),
	mEndPoint(false),
  mCurrentClickX(INT_MIN),
  mCurrentClickY(INT_MIN),
  mBorder(5)
{
  this->setObjectName("TransferFunctionAlphaWidget");
  mCurrentAlphaPoint.reset();
}
TransferFunctionAlphaWidget::~TransferFunctionAlphaWidget()
{}
void TransferFunctionAlphaWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
  if(mCurrentImage == activeImage)
    return;

  mCurrentImage = activeImage;
  //TODO: call update or not ???
  this->update();

  if (!mCurrentImage)
    return;
  if ((mCurrentImage->getBaseVtkImageData()->GetScalarType() != VTK_UNSIGNED_SHORT) &&
      (activeImage->getBaseVtkImageData()->GetScalarType() != VTK_UNSIGNED_CHAR))
    ssc::messageManager()->sendError("Active image is not unsigned (8 or 16 bit). Transfer functions will not work correctly!");
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
  QWidget::mousePressEvent(event);
  mCurrentClickX = event->x();
  mCurrentClickY = event->y();

  if(event->button() == Qt::LeftButton)
  {
    this->isInsideCurrentPoint();
  }
  else if(event->button() == Qt::RightButton)
  {
    this->toggleCurrentPoint();
  }
}
void TransferFunctionAlphaWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QWidget::mouseReleaseEvent(event);

  //we no longer need these values
  mCurrentAlphaPoint.reset();
	
  //TODO do we need to render here?
  //this->update();
}
void TransferFunctionAlphaWidget::mouseMoveEvent(QMouseEvent* event)
{
  QWidget::mouseMoveEvent(event);

  if(event->buttons() == Qt::LeftButton)
  {
    // Update current screen point for use in moveCurrentAlphaPoint
    mCurrentClickX = event->x();
    mCurrentClickY = event->y();
    this->moveCurrentAlphaPoint();
  }
  // Mouse is only moved inside the widget.
  // Emit value for possible use by info widget
  else
  {
    if((event->x() >= mPlotArea.left()) &&
       (event->x() <= (mPlotArea.width()+mPlotArea.left())))
    {
      if(mCurrentImage)
      {
        mCurrentAlphaPoint.value = int(mCurrentImage->getRange() *
                                       (event->x() - mPlotArea.left()) / 
                                       static_cast<double>(mPlotArea.width()) );
        // mCurrentAlphaPoint.position set at mousePressEvent() 
        // (with isInsideCurrentPoint())
        emit positionChanged(mCurrentAlphaPoint.value);
      }
    }
  }
}
void TransferFunctionAlphaWidget::paintEvent(QPaintEvent* event)
{
  // Don't do anything before we have an image
	// May also be fixed by not calling TransferFunctionAlphaWidget constructor till we have an image
  if (!mCurrentImage)
    return;
  
  QWidget::paintEvent(event);

  QPainter painter(this);
  QPen pointPen, pointLinePen;
  
  pointPen.setColor(QColor(0, 0, 150));
  pointLinePen.setColor(QColor(150, 100, 100));
  
  // Fill with white global background color and grey plot area background color
  const QBrush frameBrush = QBrush(QColor(170, 170, 170));
  const QBrush backgroundBrush = QBrush(QColor(200, 200, 200));
  painter.fillRect(this->mFullArea, frameBrush);
  painter.fillRect(this->mPlotArea, backgroundBrush);
	
  // Draw histogram
	// with log compression
  vtkImageAccumulatePtr histogram = mCurrentImage->getHistogram();
	int histogramSize = histogram->GetComponentExtent()[1] - 
                      histogram->GetComponentExtent()[0];
  
  painter.setPen(QColor(140, 140, 210));
  
  // A more correct approach may be to sum all values that comes inside
  // a y-value instead of drawing multiple lines on the same position.
  int x = 0;
  int y = 0;
  double barHeightMult = (height() - mBorder*2) 
	/ log(histogram->GetOutput()->GetPointData()->GetScalars()->GetRange()[1]+1);
	// / double(histogram->GetOutput()->GetPointData()->GetScalars()->GetRange()[1]);
	
  double posMult = (width() - mBorder*2) / double(histogramSize);
	//for (int i = mCurrentImage->getPosMin(); i <= mCurrentImage->getPosMax(); i++)
  for (int i = mCurrentImage->getMin(); i <= mCurrentImage->getMax(); i++)//TODO: replace with above line
	{
		x = (i * posMult);// - mCurrentImage->getPosMin(); //Offset with min value
		y = log(static_cast<int*>(histogram->GetOutput()->GetScalarPointer())[i]+1) * barHeightMult;
		//y = static_cast<int*>(histogram->GetOutput()->GetScalarPointer())[i] * barHeightMult;
    if (y > 0)
      painter.drawLine(x + mBorder, height() - mBorder, 
											 x + mBorder, height() - mBorder - y);
	}

  // Go through each point and draw squares and lines

  OpacityMapPtr opacityMap = mCurrentImage->getTransferFunctions3D()->getOpacityMap();
  QPoint lastScreenPoint;
  this->mPointRects.clear();
  for (IntIntMap::iterator opPoint = opacityMap->begin();
       opPoint != opacityMap->end();
       opPoint++)
  {
    // Get the screen (plot) position of this point
    QPoint screenPoint = QPoint(
      static_cast<int>(mPlotArea.left() + mPlotArea.width() * 
                       opPoint->first / 
                       static_cast<double>(mCurrentImage->getRange())),
      static_cast<int>(mPlotArea.bottom() - mPlotArea.height() * 
                       opPoint->second / 
                       static_cast<double>(mCurrentImage->getMaxAlphaValue())) );
		
		//std::cout << "x: " << opPoint->first  << " y: " << opPoint->second << std::endl;
		//std::cout << "Screen x: " << screenPoint.x() << " y: " << screenPoint.y() << std::endl;
		// Draw line from previous point if this is not the first point
		if (opPoint != opacityMap->begin())
		{
			painter.setPen(pointLinePen);
			painter.drawLine(lastScreenPoint, screenPoint);
		}
		
		// Draw the rectangle
		QRect pointRect(screenPoint.x() - mBorder, screenPoint.y() - mBorder, 
										mBorder*2, mBorder*2);
		painter.setPen(pointPen);
		painter.drawRect(pointRect);
		this->mPointRects[opPoint->first] = pointRect;
		
		// Store the point
		lastScreenPoint = screenPoint;
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
bool TransferFunctionAlphaWidget::isInsideCurrentPoint()
{
	mEndPoint = false;
  std::map<int, QRect>::iterator it = mPointRects.begin();
  for(;it != mPointRects.end(); ++it)
  {
    if (it->second.contains(mCurrentClickX, mCurrentClickY))
    {
      mCurrentAlphaPoint.position = it->first;
			if (it == mPointRects.begin() || it == --mPointRects.end())
				mEndPoint = true;
      return true;
    }
  }
  mCurrentAlphaPoint.reset();
  return false;
}
TransferFunctionAlphaWidget::AlphaPoint TransferFunctionAlphaWidget::getCurrentAlphaPoint()
{
  AlphaPoint point;

  point.position = 
    static_cast<int>( mCurrentImage->getRange() * 
                     (mCurrentClickX - mPlotArea.left()) / 
                     static_cast<double>(mPlotArea.width()) );
  point.value = 
    static_cast<int>( mCurrentImage->getMaxAlphaValue() * 
                     (mPlotArea.bottom() - mCurrentClickY) / 
                     static_cast<double>(mPlotArea.height()) );

  /*if (point.position > mCurrentImage->getPosMax())
    point.position = mCurrentImage->getPosMax();
  else if (point.position < mCurrentImage->getPosMin())
    point.position = mCurrentImage->getPosMin();*/
  //TODO: replace code with above code
  if (point.position > mCurrentImage->getMax())
    point.position = mCurrentImage->getMax();
  else if (point.position < mCurrentImage->getMin())
    point.position = mCurrentImage->getMin();

  if (point.value > mCurrentImage->getMaxAlphaValue())
    point.value = mCurrentImage->getMaxAlphaValue();
  else if (point.value < 0)
    point.value = 0;

  return point;
}
void TransferFunctionAlphaWidget::toggleCurrentPoint()
{
  if(!mCurrentImage)
    return;
  if(!isInsideCurrentPoint())
  {
    // Outside any of the rectangles
    AlphaPoint point = getCurrentAlphaPoint();
    mCurrentImage->getTransferFunctions3D()->addAlphaPoint(point.position,point.value);
  }
	// mEndPoint is set in isInsideCurrentPoint()
  else if(!mEndPoint)
  {
    // Inside one of the rectangles
    if(mCurrentAlphaPoint.isValid())
      mCurrentImage->getTransferFunctions3D()->removeAlphaPoint(mCurrentAlphaPoint.position);
  }

  this->update();
}
void TransferFunctionAlphaWidget::moveCurrentAlphaPoint()
{
  if(!mCurrentAlphaPoint.isValid())
    return;

  AlphaPoint newAlphaPoint = this->getCurrentAlphaPoint();

  ssc::ImageTF3DPtr transferFunction = mCurrentImage->getTransferFunctions3D();
  
  // Max and min points may only be moved in y direction
  /*if(mCurrentAlphaPoint.position == mCurrentImage->getPosMin() 
     || mCurrentAlphaPoint.position == mCurrentImage->getPosMax() )*/
  //TODO: Replace with above code
  if(mCurrentAlphaPoint.position == mCurrentImage->getMin() 
     || mCurrentAlphaPoint.position == mCurrentImage->getMax() )
  {
    transferFunction->setAlphaValue(mCurrentAlphaPoint.position, 
                                    newAlphaPoint.value);
  }
  else
  {
    OpacityMapPtr opacityMap = transferFunction->getOpacityMap();
    IntIntMap::iterator prevpoint = opacityMap->find(mCurrentAlphaPoint.position);
    IntIntMap::iterator nextpoint = opacityMap->find(mCurrentAlphaPoint.position);
    prevpoint--;
    nextpoint++;

    if (newAlphaPoint.position <= prevpoint->first)
      newAlphaPoint.position = prevpoint->first + 1;
    else if (newAlphaPoint.position >= nextpoint->first)
      newAlphaPoint.position = nextpoint->first - 1;

    transferFunction->removeAlphaPoint(mCurrentAlphaPoint.position);
    transferFunction->addAlphaPoint(newAlphaPoint.position, newAlphaPoint.value);

    mCurrentAlphaPoint = newAlphaPoint;
  }
	// Update GUI while moving point
	this->update();
}
}//namespace cx
