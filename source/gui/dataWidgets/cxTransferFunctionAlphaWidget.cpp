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
#include "sscImageTFData.h"
#include "sscMessageManager.h"
#include "sscUtilHelpers.h"
#include "sscTypeConversions.h"

namespace cx
{
TransferFunctionAlphaWidget::TransferFunctionAlphaWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunctionAlphaWidget", "Alpha Transfer Function"),
  mEndPoint(false),
  mBorder(5),
  mReadOnly(false)
{
  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageTransferFunctionsChangedSlot()));

  mCurrentAlphaPoint.reset();
}
TransferFunctionAlphaWidget::~TransferFunctionAlphaWidget()
{}

QString TransferFunctionAlphaWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Alpha Transfer Function.</h3>"
    "<p>Lets you set the alpha part of a transfer function.</p>"
    "<p><i></i></p>"
    "</html>";
}

void TransferFunctionAlphaWidget::setData(ssc::ImagePtr image, ssc::ImageTFDataPtr tfData)
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
    this->isInsideCurrentPoint(event->x(), event->y());
  }
  else if(event->button() == Qt::RightButton)
  {
    this->toggleCurrentPoint(event->x(), event->y());
  }
}

void TransferFunctionAlphaWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if(mReadOnly)
    return;
  QWidget::mouseReleaseEvent(event);

  //we no longer need these values
  mCurrentAlphaPoint.reset();
	
  //TODO do we need to render here?
  //this->update();
}

void TransferFunctionAlphaWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!mImage)
    return;

  AlphaPoint point = this->getCurrentAlphaPoint(event->x(), event->y());
  this->setToolTip(QString("(%1, %2)").arg(point.position).arg(point.value / 255.0, 0, 'f', 2));
  if(mReadOnly) //Only show tool tip if readOnly
    return;
  QWidget::mouseMoveEvent(event);

  if(event->buttons() == Qt::LeftButton)
  {
    this->moveCurrentAlphaPoint(event->x(), event->y());
  }
  // Mouse is only moved inside the widget.
  // Emit value for possible use by info widget
  else
  {
    if((event->x() >= mPlotArea.left()) &&
       (event->x() <= (mPlotArea.width()+mPlotArea.left())))
    {
      if(mImage)
      {
        mCurrentAlphaPoint.position = int(mImage->getMin() + mImage->getRange() *
                                       (event->x() - mPlotArea.left()) / 
                                       static_cast<double>(mPlotArea.width()) );
        emit positionChanged(mCurrentAlphaPoint.position);
      }
    }
  }
}
void TransferFunctionAlphaWidget::paintEvent(QPaintEvent* event)
{
  // Don't do anything before we have an image
  // May also be fixed by not calling TransferFunctionAlphaWidget constructor till we have an image
  if (!mImage)
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

  vtkImageAccumulatePtr histogram = mImage->getHistogram();
  int histogramSize = histogram->GetComponentExtent()[1] - 
                      histogram->GetComponentExtent()[0];
  
  painter.setPen(QColor(140, 140, 210));
  
  // A more correct approach may be to sum all values that comes inside
  // a y-value instead of drawing multiple lines on the same position.
  int x = 0;
  int y = 0;
  double barHeightMult = (height() - mBorder*2) 
  / log(histogram->GetOutput()->GetPointData()->GetScalars()->GetRange()[1]+1);
  
  double posMult = (width() - mBorder*2) / double(histogramSize);
  for (int i = mImage->getMin(); i <= mImage->getMax(); i++)
  {
    x = ((i- mImage->getMin()) * posMult); //Offset with min value
    y = log(double(static_cast<int*>(histogram->GetOutput()->GetScalarPointer())[i - mImage->getMin()]+1)) * barHeightMult;
    if (y > 0)
    {
      painter.drawLine(x + mBorder, height() - mBorder, 
                       x + mBorder, height() - mBorder - y);
    }
  }

  // Go through each point and draw squares and lines
  ssc::OpacityMapPtr opacityMap = mImageTF->getOpacityMap();

  QPoint lastScreenPoint;
  this->mPointRects.clear();
  for (ssc::IntIntMap::iterator opPoint = opacityMap->begin();
       opPoint != opacityMap->end();
       opPoint++)
  {
    // Get the screen (plot) position of this point
    QPoint screenPoint = QPoint(
      static_cast<int>(mPlotArea.left() + mPlotArea.width() * 
                       (opPoint->first - mImage->getMin()) /
                       static_cast<double>(mImage->getRange())),
      static_cast<int>(mPlotArea.bottom() - mPlotArea.height() * 
                       opPoint->second / 
                       static_cast<double>(mImage->getMaxAlphaValue())) );

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

bool TransferFunctionAlphaWidget::isInsideCurrentPoint(int mouseX, int mouseY)
{
  mEndPoint = false;
  std::map<int, QRect>::iterator it = mPointRects.begin();
  for(;it != mPointRects.end(); ++it)
  {
    if (it->second.contains(mouseX, mouseY))
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

TransferFunctionAlphaWidget::AlphaPoint TransferFunctionAlphaWidget::getCurrentAlphaPoint(int mouseX, int mouseY)
{
  AlphaPoint point;

  point.position = 
    static_cast<int>(mImage->getMin() + ( mImage->getRange() *
                     (mouseX - mPlotArea.left()) /
                     static_cast<double>(mPlotArea.width()) ));
  point.value = 
    static_cast<int>( mImage->getMaxAlphaValue() *
                     (mPlotArea.bottom() - mouseY) /
                     static_cast<double>(mPlotArea.height()) );

  point.position = ssc::constrainValue(point.position, mImage->getMin(), mImage->getMax());
  point.value = ssc::constrainValue(point.value, 0, mImage->getMaxAlphaValue());

  return point;
}

void TransferFunctionAlphaWidget::toggleCurrentPoint(int mouseX, int mouseY)
{
  if(!mImage)
    return;
  if(!isInsideCurrentPoint(mouseX, mouseY))
  {
    // Outside any of the rectangles
    AlphaPoint point = getCurrentAlphaPoint(mouseX, mouseY);
    mImageTF->addAlphaPoint(point.position,point.value);
  }
  // mEndPoint is set in isInsideCurrentPoint()
  else if(!mEndPoint)
  {
    // Inside one of the rectangles
    if(mCurrentAlphaPoint.isValid())
      mImageTF->removeAlphaPoint(mCurrentAlphaPoint.position);
  }

  this->update();
}

void TransferFunctionAlphaWidget::moveCurrentAlphaPoint(int mouseX, int mouseY)
{
  if(!mCurrentAlphaPoint.isValid())
    return;

  AlphaPoint newAlphaPoint = this->getCurrentAlphaPoint(mouseX, mouseY);

  // Max and min points may only be moved in y direction
  if(mCurrentAlphaPoint.position == mImage->getMin()
     || mCurrentAlphaPoint.position == mImage->getMax() )
  {
    mImageTF->addAlphaPoint(mCurrentAlphaPoint.position,
                                    newAlphaPoint.value);
  }
  else
  {
    ssc::OpacityMapPtr opacityMap = mImageTF->getOpacityMap();
    ssc::IntIntMap::iterator prevpoint = opacityMap->find(mCurrentAlphaPoint.position);
    ssc::IntIntMap::iterator nextpoint = opacityMap->find(mCurrentAlphaPoint.position);
    prevpoint--;
    nextpoint++;

    if (newAlphaPoint.position <= prevpoint->first)
      newAlphaPoint.position = prevpoint->first + 1;
    else if (newAlphaPoint.position >= nextpoint->first)
      newAlphaPoint.position = nextpoint->first - 1;

    mImageTF->removeAlphaPoint(mCurrentAlphaPoint.position);
    mImageTF->addAlphaPoint(newAlphaPoint.position, newAlphaPoint.value);

    mCurrentAlphaPoint = newAlphaPoint;
  }
  // Update GUI while moving point
  this->update();
}
}//namespace cx
