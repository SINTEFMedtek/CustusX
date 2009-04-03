#include "cxTransferFunctionAlphaWidget.h"

#include <limits.h>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QBrush>
#include <QMouseEvent>

namespace cx
{
TransferFunctionAlphaWidget::TransferFunctionAlphaWidget(QWidget* parent) :
  QWidget(parent),
  mCurrentClickX(INT_MIN),
  mCurrentClickY(INT_MIN),
  mBorder(5)
{
  mCurrentAlphaPoint.reset();
}
TransferFunctionAlphaWidget::~TransferFunctionAlphaWidget()
{}
void TransferFunctionAlphaWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if(mCurrentImage == currentImage)
    return;

  mCurrentImage = currentImage;
  //TODO: call update or not ???
  update();
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

  this->update();

  //TODO do we need to render here?
}
void TransferFunctionAlphaWidget::mouseMoveEvent(QMouseEvent* event)
{
  QWidget::mouseMoveEvent(event);

  if(event->button() != Qt::NoButton)
  {
    // TODO: Check if this works or if x and y must be input values in moveCurrentAlphaPoint
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
      mCurrentAlphaPoint.value = int(mCurrentImage->getAlphaRange() *
          (event->x() - mPlotArea.left()) / 
          static_cast<double>(mPlotArea.width()) );
      // mCurrentAlphaPoint.position set at mousePressEvent() 
      // (with isInsideCurrentPoint())
      emit positionChanged(mCurrentAlphaPoint.value);
    }
  }
}
void TransferFunctionAlphaWidget::paintEvent(QPaintEvent* event)
{
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
  ssc::HistogramMapPtr histogram = mCurrentImage->getHistogram();
  int histogramSize = histogram->end()->first - histogram->begin()->first;
  ssc::IntIntMap::iterator iter;// = histogram->begin();
  
  painter.setPen(QColor(140, 140, 210));
  
  // A more correct approach may be to sum all values that comes inside
  // a y-value instead of drawing multiple lines on the same position.
  int x = 0;
  int y = 0;
  double barHeightMult = (height() - mBorder*2) / mCurrentImage->getMaxHistogramValue();
  double posMult = (width() - mBorder*2) / histogramSize;
  for (iter = histogram->begin(); iter!= histogram->end(); iter++)
  {
    x = static_cast<int>((iter->first - histogram->end()->first) * posMult);
    y = static_cast<int>(iter->second * barHeightMult);
    
    if (y > 0)
      painter.drawLine(x, height() - mBorder, x, height() - mBorder - y);
  }

  // Go through each point and draw squares and lines

  ssc::OpacityMapPtr opacityMap = mCurrentImage->getTransferFunctions3D().getOpacityMap();
  QPoint lastScreenPoint;
  this->mPointRects.clear();
  for (ssc::IntIntMap::iterator opPoint = opacityMap->begin();
       opPoint != opacityMap->end();
       opPoint++)
  {
    // Get the screen (plot) position of this point
    // getMaxAlphaValue() must return max alpha value 
    // (should probably be in sscImageFT3D)
    // getMax() returns max alpha position
    // getAlphaRange() returns max - min alpha position
    QPoint screenPoint = QPoint(
      static_cast<int>(mPlotArea.left() + mPlotArea.width() * 
                       opPoint->first / 
                       static_cast<double>(mCurrentImage->getAlphaRange())),
      static_cast<int>(mPlotArea.right() + mPlotArea.height() * 
                       opPoint->second / 
                       static_cast<double>(mCurrentImage->getMaxAlphaValue())) );
  
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
  std::map<int, QRect>::iterator it = mPointRects.begin();
  for(;it != mPointRects.end(); ++it)
  {
    if (it->second.contains(mCurrentClickX, mCurrentClickY))
    {
      mCurrentAlphaPoint.position = it->first;
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
    static_cast<int>( mCurrentImage->getAlphaRange() * 
                     (mCurrentClickX - mPlotArea.left()) / 
                     static_cast<double>(mPlotArea.width()) );
  point.value = 
    static_cast<int>( mCurrentImage->getMaxAlphaValue() * 
                     (mPlotArea.bottom() - mCurrentClickY) / 
                     static_cast<double>(mPlotArea.height()) );

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
  if(!isInsideCurrentPoint())
  {
    // Outside any of the rectangles
    AlphaPoint point = getCurrentAlphaPoint();
    mCurrentImage->getTransferFunctions3D().addAlphaPoint(point.position,point.value);
  }
  else
  {
    // Inside one of the rectangles
    if(mCurrentAlphaPoint.isValid())
      mCurrentImage->getTransferFunctions3D().removeAlphaPoint(mCurrentAlphaPoint.position);
  }

  this->update();
}
void TransferFunctionAlphaWidget::moveCurrentAlphaPoint()
{
  if(!mCurrentAlphaPoint.isValid())
    return;

  AlphaPoint newAlphaPoint = this->getCurrentAlphaPoint();

  ssc::ImageTF3D transferFunction = mCurrentImage->getTransferFunctions3D();
  
  // Max and min points may only be moved in y direction
  if(mCurrentAlphaPoint.position == mCurrentImage->getMin() 
     || mCurrentAlphaPoint.position == mCurrentImage->getMax() )
  {
    transferFunction.setAlphaValue(mCurrentAlphaPoint.position, 
                                    newAlphaPoint.value);
  }
  else
  {
    ssc::OpacityMapPtr opacityMap = transferFunction.getOpacityMap();
    ssc::IntIntMap::iterator prevpoint = opacityMap->find(mCurrentAlphaPoint.position);
    ssc::IntIntMap::iterator nextpoint = opacityMap->find(mCurrentAlphaPoint.position);
    prevpoint--;
    nextpoint++;

    if (newAlphaPoint.position <= prevpoint->first)
      newAlphaPoint.position = prevpoint->first + 1;
    else if (newAlphaPoint.position >= nextpoint->first)
      newAlphaPoint.position = nextpoint->first - 1;

    transferFunction.removeAlphaPoint(mCurrentAlphaPoint.position);
    transferFunction.addAlphaPoint(newAlphaPoint.position, newAlphaPoint.value);

    mCurrentAlphaPoint = newAlphaPoint;

  }
}
}//namespace cx
