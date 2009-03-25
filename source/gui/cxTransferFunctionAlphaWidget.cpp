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
/*  mCurrentAlphaValue(INT_MIN),
  mCurrentAlphaPosition(INT_MIN),*/
  mCurrentClickX(INT_MIN),
  mCurrentClickY(INT_MIN)
{}
TransferFunctionAlphaWidget::~TransferFunctionAlphaWidget()
{}
void TransferFunctionAlphaWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if(mCurrentImage == currentImage)
    return;

  mCurrentImage = currentImage;
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
    this->moveCurrentAlphaPoint();
  }
  else
  {
    if((event->x() >= mPlotArea.left()) &&
      (event->x() <= (mPlotArea.width()+mPlotArea.left())))
    {
/*      mCurrentAlphaPoint.value = int(mCurrentImage->getMaxAlphaPosition() *
          (event->x() - mPlotArea.left()) / float(mPlotArea.width()) );*/
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
/*  int lastindex = 0;
  painter.setPen(QColor(140, 140, 210));
  for (int x = 5; x < width() - 5; ++x)
  {
    int histindex =
        static_cast<int>(0.5+(histogram_.size()-1)*(x-5.0)/(width()-10.0) );
//    int y = static_cast<int>(histogram_[histindex]*barheightmult_);
  int y = 0;
  if(histindex == lastindex)
    y = static_cast<int>(histogram_[histindex]*barheightmult_ + 0.5);
  else
  {
//    int maxelement = 0;
//    for(int i = lastindex+1; i <= histindex; i++)
//    maxelement = std::max(maxelement, histogram_[i]);
    int maxelement = *std::max_element(&histogram_[lastindex+1],
                     &histogram_[histindex+1]);
    y = static_cast<int>(maxelement * barheightmult_ + 0.5);
  }
  lastindex = histindex;
    if (y > 0)
      painter.drawLine(x, height() - 5, x, height() - 5 - y);
  }*/

  // Go through each point and draw squares and lines
/*
  QPoint lastPos;
  this->mPointRects.clear();
  for (std::map<int, int>::iterator dizpoint =
     transfx_->getAlphaPoints()->begin();
     (dizpoint != transfx_->getAlphaPoints()->end()) &&
     (dizpoint->first <= transfx_->getMaxValue());
     dizpoint++)
  {
    // Get the scalar and value of this point
    int iScalar = dizpoint->first;
    int iValue = dizpoint->second;

    // Get the screen (plot) position of this point
    //QPoint dizPos = getPlotPoint(iScalar, iValue); TODO

    // Draw line from previous point if this is not the first point
    if (dizpoint != transfx_->getAlphaPoints()->begin())
    {
      painter.setPen(pointLinePen);
      //painter.drawLine(lastPos, dizPos); TODO
    }

    // Draw the rectangle
    int iSquareSize = 10;
    //QRect pointrect(dizPos.x()-iSquareSize/2, dizPos.y()-iSquareSize/2, iSquareSize, iSquareSize); //TODO

    painter.setPen(pointPen);
    //painter.drawRect(pointrect); //TODO

    //this->mPointRects[iScalar] = pointrect; TODO

    // Store the point
    //lastPos = dizPos; TODO
*/
}
void TransferFunctionAlphaWidget::resizeEvent(QResizeEvent* evt)
{
  QWidget::resizeEvent(evt);

  // Calculate areas
  this->mFullArea = QRect(0, 0, width(), height());
  this->mPlotArea = QRect(5, 5, width() - 10, height() - 10);

  //barwidthmult_ = (width() - 10) / static_cast<double>(histogram_.size());
  //barheightmult_ = static_cast<double>( height() - 10 ) / histmax_;
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

/* TODO
  point.alphaPosition = int( transfx_->getMaxValue() * (iX - plotarea.left()) / float(plotarea.width()) );
  point.alphaValue = int( transfx_->getMaxAlpha() * (plotarea.bottom() - iY) / float(plotarea.height()) );

  if (point.alphaPosition > transfx_->getMaxValue())
    point.alphaPosition = transfx_->getMaxValue();
  else if (point.alphaPosition < 0)
    point.alphaPosition = 0;

  if (point.alphaValue > transfx_->getMaxAlpha())
    point.alphaValue = transfx_->getMaxAlpha();
  else if (point.alphaValue < 0)
    point.alphaValue = 0;
*/

  return (point);
}
void TransferFunctionAlphaWidget::toggleCurrentPoint()
{
  if(!isInsideCurrentPoint())
  {
    // Outside any of the rectangles
    AlphaPoint point = getCurrentAlphaPoint();
    //TODO add point to mCurrentImage
    //mCurrentImage->addAlphaValue(point.alphaPosition,point.alphaValue);
  }
  else
  {
    // Inside one of the rectangles
    //TODO remove point from mCurrentImage
    //if(mCurrentAlphaPosition != INT_MIN)
      //mCurrentImage->removeAlphaValue(mCurrentAlphaPosition);
  }

  this->update();
}
void TransferFunctionAlphaWidget::moveCurrentAlphaPoint()
{
  if(!mCurrentAlphaPoint.isValid())
    return;

  AlphaPoint alphaPoint = this->getCurrentAlphaPoint();

  if(mCurrentAlphaPoint.position == 0  )
    // || mCurrentAlphaPoint.position == mCurrentImage->getMaxAlphaPosition() ) TODO
  {
/*    mCurrentImage->getTransferFunctions3D()->setAlphaValue(mCurrentAlphaPoint.position,
                                                           mCurrentAlphaPoint.value);*/
  }
  else
  {
/*    std::map<int, int>::iterator prevpoint = mCurrentImage->getAlphaPoints()->find(mCurrentAlphaPoint.position);
    std::map<int, int>::iterator nextpoint = mCurrentImage->getAlphaPoints()->find(mCurrentAlphaPoint.position);
    prevpoint--;
    nextpoint++;

    if (alphaPoint.position <= prevpoint->first)
      alphaPoint.position = prevpoint->first + 1;
    else if (alphaPoint.position >= nextpoint->first)
      alphaPoint.position = nextpoint->first - 1;

    mCurrentImage->getAlphaPoints()->removeAlphaPoint(mCurrentAlphaPoint.position);
    mCurrentImage->getAlphaPoints()->addAlphaPoint(alphaPoint.position, alphaPoint.value);*/

    mCurrentAlphaPoint = alphaPoint;

  }
}
}//namespace cx
