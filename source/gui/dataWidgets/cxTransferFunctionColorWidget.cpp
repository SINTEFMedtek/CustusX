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
#include "sscDataManager.h"
#include "sscImageTF3D.h"
#include "sscImageTFData.h"
#include "sscUtilHelpers.h"
#include "sscImageLUT2D.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{
TransferFunctionColorWidget::TransferFunctionColorWidget(QWidget* parent) :
  BaseWidget(parent, "TransferFunctionColorWidget", "Color Transfer Function"),
	mCurrentClickPos(INT_MIN,INT_MIN),
  mBorder(5)
{
  mActiveImageProxy = ActiveImageProxy::New();
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

QString TransferFunctionColorWidget::defaultWhatsThis() const
{
  return "<html>"
	"<h3>Color Transfer Function</h3>"
	"<p>Let you set the color part of a transfer function.</p>"
    "<p><i></i></p>"
    "</html>";
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
  }
}

void TransferFunctionColorWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QWidget::mouseReleaseEvent(event);
  //we no longer need these values
  mSelectedPoint.reset();
}

void TransferFunctionColorWidget::mouseMoveEvent(QMouseEvent* event)
{
  QWidget::mouseMoveEvent(event);

  if(event->buttons() == Qt::LeftButton)
  {
    // Update current screen point for use in moveCurrentAlphaPoint
	mCurrentClickPos = event->pos();
    this->moveCurrentPoint();
  }
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
	int retval =
			static_cast<int>( mImage->getMin() + ( mImage->getRange() ) *
							  (screenX - mPlotArea.left()) /
							  static_cast<double>(mPlotArea.width()-1) );
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
	QPen pointPen;
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
		painter.setPen(pointPen);
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

  ColorPoint point = this->selectPoint(mCurrentClickPos);

  if (point.isValid() && !this->isEndpoint(point.intensity))
  {
    menu.addSeparator();
    menu.addAction(mRemoveColorAction);
  }
  menu.exec(event->globalPos());
}
  
TransferFunctionColorWidget::ColorPoint TransferFunctionColorWidget::getCurrentColorPoint()
{
  ColorPoint point;
  point.intensity = screenX2imageIntensity(mCurrentClickPos.x());
  point.intensity = constrainValue(point.intensity, mImage->getMin(), mImage->getMax());

  vtkColorTransferFunctionPtr trFunc = mImageTF->generateColorTF();

  double* rgb = trFunc->GetColor(point.intensity);
  point.value = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
  return point;
}

void TransferFunctionColorWidget::moveCurrentPoint()
{
	if(!mSelectedPoint.isValid())
		return;

//	if (this->isEndpoint(mSelectedPoint.intensity))
//		return;

	std::pair<int,int> range = this->findAllowedMoveRangeAroundColorPoint(mSelectedPoint.intensity);

	ColorPoint newColorPoint = this->getCurrentColorPoint();
	newColorPoint.value = mSelectedPoint.value;
	newColorPoint.intensity = constrainValue(newColorPoint.intensity, range.first, range.second);

	mImageTF->moveColorPoint(mSelectedPoint.intensity, newColorPoint.intensity, newColorPoint.value);
	mSelectedPoint = newColorPoint;
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
