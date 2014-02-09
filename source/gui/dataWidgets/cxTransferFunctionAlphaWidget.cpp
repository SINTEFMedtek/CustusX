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
	  this->isInsideCurrentPoint(event->pos());
  }
  else if(event->button() == Qt::RightButton)
  {
	  this->toggleCurrentPoint(event->pos());
  }
}

void TransferFunctionAlphaWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if(mReadOnly)
    return;
  QWidget::mouseReleaseEvent(event);
  mCurrentAlphaPoint.reset();
}

void TransferFunctionAlphaWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!mImage)
    return;

  AlphaPoint point = this->getCurrentAlphaPoint(event->pos());
  this->setToolTip(QString("(%1, %2)").arg(point.position).arg(point.value / 255.0, 0, 'f', 2));
  if(mReadOnly) //Only show tool tip if readOnly
    return;
  QWidget::mouseMoveEvent(event);

  if(event->buttons() == Qt::LeftButton)
  {
	this->moveCurrentAlphaPoint(event->pos());
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
  
  // Fill with white global background color and grey plot area background color
  const QBrush frameBrush = QBrush(QColor(170, 170, 170));
  const QBrush backgroundBrush = QBrush(QColor(200, 200, 200));
  painter.fillRect(this->mFullArea, frameBrush);
  painter.fillRect(this->mPlotArea, backgroundBrush);

  this->paintHistogram(painter);
  this->paintOpacityGraph(painter);
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
			lastScreenPoint = QPoint(mImage->getMin(), screenPoint.y());
		}

	  // Draw line from previous point
		painter.setPen(pointLinePen);
		painter.drawLine(lastScreenPoint, screenPoint);

	  // Draw the rectangle
	  QRect pointRect(screenPoint.x() - mBorder, screenPoint.y() - mBorder,
					  mBorder*2, mBorder*2);
	  painter.setPen(pointPen);
	  painter.drawRect(pointRect);
	  this->mPointRects[opPoint->first] = pointRect;

	  // Store the point
	  lastScreenPoint = screenPoint;
	}

	// draw a line from the last point to the right end
	QPoint screenPoint(mImage->getMax(), lastScreenPoint.y());
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

	int x = 0;
	int y = 0;
	double barHeightMult = (this->height() - mBorder*2)
	/ log(histogram->GetOutput()->GetPointData()->GetScalars()->GetRange()[1]+1);

	double posMult = (this->width() - mBorder*2) / double(histogramSize);
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
}


void TransferFunctionAlphaWidget::resizeEvent(QResizeEvent* evt)
{
  QWidget::resizeEvent(evt);

  // Calculate areas
  this->mFullArea = QRect(0, 0, width(), height());
  this->mPlotArea = QRect(mBorder, mBorder, 
                          width() - mBorder*2, height() - mBorder*2);
}

bool TransferFunctionAlphaWidget::isInsideCurrentPoint(QPoint pos)
{
  std::map<int, QRect>::iterator it = mPointRects.begin();
  for(;it != mPointRects.end(); ++it)
  {
	if (it->second.contains(pos))
    {
      mCurrentAlphaPoint.position = it->first;
      return true;
    }
  }
  mCurrentAlphaPoint.reset();
  return false;
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

  point.position = 
    static_cast<int>(mImage->getMin() + ( mImage->getRange() *
					 (pos.x() - mPlotArea.left()) /
                     static_cast<double>(mPlotArea.width()) ));
  point.value = 
    static_cast<int>( mImage->getMaxAlphaValue() *
					 (mPlotArea.bottom() - pos.y()) /
                     static_cast<double>(mPlotArea.height()) );

  point.position = constrainValue(point.position, mImage->getMin(), mImage->getMax());
  point.value = constrainValue(point.value, 0, mImage->getMaxAlphaValue());

  return point;
}

void TransferFunctionAlphaWidget::toggleCurrentPoint(QPoint pos)
{
  if(!mImage)
    return;
  if(!isInsideCurrentPoint(pos))
  {
    // Outside any of the rectangles
	AlphaPoint point = getCurrentAlphaPoint(pos);
    mImageTF->addAlphaPoint(point.position,point.value);
  }
  else if(!this->isEndpoint(mCurrentAlphaPoint.position))
  {
    // Inside one of the rectangles
    if(mCurrentAlphaPoint.isValid())
      mImageTF->removeAlphaPoint(mCurrentAlphaPoint.position);
  }

  this->update();
}

void TransferFunctionAlphaWidget::moveCurrentAlphaPoint(QPoint pos)
{
	if(!mCurrentAlphaPoint.isValid())
		return;

	AlphaPoint newAlphaPoint = this->getCurrentAlphaPoint(pos);

	std::pair<int,int> range = this->findAllowedMoveRangeAroundAlphaPoint(mCurrentAlphaPoint.position);
	newAlphaPoint.position = constrainValue(newAlphaPoint.position, range.first, range.second);

	mImageTF->moveAlphaPoint(mCurrentAlphaPoint.position, newAlphaPoint.position, newAlphaPoint.value);

	mCurrentAlphaPoint = newAlphaPoint;
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
