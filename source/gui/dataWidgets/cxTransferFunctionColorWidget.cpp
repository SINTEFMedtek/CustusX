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

namespace cx
{
TransferFunctionColorWidget::TransferFunctionColorWidget(QWidget* parent) :
  QWidget(parent),
	mEndPoint(false),
  mColorindexSelected(0),
  mCurrentClickX(INT_MIN),
  mCurrentClickY(INT_MAX),
  mBorder(5)
{
  this->setObjectName("TransferFunctionColorWidget");
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()),
          this, SLOT(activeImageTransferFunctionsChangedSlot()));

  mCurrentPoint.reset();
  
  // Create actions
  mCustomColorAction = new QAction(tr("Custom color..."), this);
  mRemoveColorAction = new QAction(tr("Remove point"), this);
  
  connect(mCustomColorAction, SIGNAL(triggered(bool)), this, SLOT(setColorSlot()));
  connect(mRemoveColorAction, SIGNAL(triggered()), this, SLOT(removeColorSlot()));
}

TransferFunctionColorWidget::~TransferFunctionColorWidget()
{}

void TransferFunctionColorWidget::setData(ssc::ImagePtr image, ssc::ImageTFDataPtr tfData)
{
  if (( mImage == image )&&( mImageTF==tfData ))
    return;

  mImage = image;
  mImageTF = tfData;
  this->update();
}

//void TransferFunctionColorWidget::activeImageChangedSlot()
//{
//  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
//  if(mCurrentImage == activeImage)
//    return;
//
//  mCurrentImage = activeImage;
//  //TODO: call update or not ???
//  this->update();
//}
  
void TransferFunctionColorWidget::activeImageTransferFunctionsChangedSlot()
{
  this->update();
}
  
void TransferFunctionColorWidget::mousePressEvent(QMouseEvent* event)
{
  QWidget::mousePressEvent(event);
  mCurrentClickX = event->x();
  mCurrentClickY = event->y();

  if(event->button() == Qt::LeftButton)
  {
    this->isInsideCurrentPoint();
  }
  // contextMenuEvent() is automatically called on right click
  /*else if(event->button() == Qt::RightButton)
  {
    if(this->isInsideCurrentPoint())
      this->contextMenuEvent(event);
  }*/
}
void TransferFunctionColorWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QWidget::mouseReleaseEvent(event);

  //we no longer need these values
  mCurrentPoint.reset();

  //TODO do we need to render here?
  //this->update();
}
void TransferFunctionColorWidget::mouseMoveEvent(QMouseEvent* event)
{
  QWidget::mouseMoveEvent(event);

  if(event->buttons() == Qt::LeftButton)
  {
    // TODO: Check if this works or if x and y must be input values in moveCurrentAlphaPoint
    // Update current screen point for use in moveCurrentAlphaPoint
    mCurrentClickX = event->x();
    mCurrentClickY = event->y();
    this->moveCurrentPoint();
  }
}

void TransferFunctionColorWidget::calculateColorTFBoundaries(int &areaLeft, int &areaRight, int &areaWidth)
{

  double min = mImageTF->getLevel() - ( mImageTF->getWindow() / 2.0 ) - mImage->getMin();
  double max = mImageTF->getLevel() + ( mImageTF->getWindow() / 2.0 ) - mImage->getMin();

  areaLeft  = mPlotArea.left()  + (min * mPlotArea.width() / mImage->getRange());
  areaRight = mPlotArea.left()  + (max * mPlotArea.width() / mImage->getRange());
  if(mImage->getRange() == 0) // Fix division by zero problem
  {
    areaLeft = mPlotArea.left();
    areaRight = mPlotArea.right();
    //ssc::messageManager()->sendWarning("Volume only have one value");
  }
  areaWidth = areaRight - areaLeft;
}

int TransferFunctionColorWidget::calculateXPositionInTrFunc(int screenX)
{
  int areaLeft, areaRight, areaWidth;
  this->calculateColorTFBoundaries(areaLeft, areaRight, areaWidth);
  int retval =
//    static_cast<int>(0.5 + mImage->getMin() + ( mImage->getRange() -1) *
//                     (screenX - areaLeft) /
//                     static_cast<double>(areaWidth-1) );
  static_cast<int>( mImage->getMin() + ( mImage->getRange() ) *
                   (screenX - areaLeft) /
                   static_cast<double>(areaWidth-1) );
  return retval;
}

void TransferFunctionColorWidget::paintEvent(QPaintEvent* event)
{
	// Don't do anything before we have an image
	if (!mImage)
		return;
  QWidget::paintEvent(event);

  QPainter painter(this);
  QPen pointPen, pointLinePen;

  // Fill with white background color and grey plot area background color
  painter.fillRect(this->mFullArea, QColor(170, 170, 170));
  painter.fillRect(this->mPlotArea, QColor(200, 200, 200));

  // Draw color-background

//  ssc::ImageTF3DPtr transferFunction = mCurrentImage->getTransferFunctions3D();

  painter.setPen(QColor(140, 140, 210));

	// Use vtkColorTransferFunction for interpolation
  vtkColorTransferFunctionPtr trFunc = vtkColorTransferFunctionPtr::New();
	mImageTF->fillColorTFFromMap(trFunc);

	//Use window/level to find new left and right values
	int areaLeft, areaRight, areaWidth;
	this->calculateColorTFBoundaries(areaLeft, areaRight, areaWidth);

  for (int x = areaLeft; x <= areaRight; ++x)
  {
    int point = calculateXPositionInTrFunc(x);

    //QColor color = transferFunction->getInterpolatedColorValue(point);
		double* rgb = trFunc->GetColor(point);
    painter.setPen(QColor(int(rgb[0]*255), int(rgb[1]*255), int(rgb[2]*255)));
    painter.drawLine(x, mPlotArea.top(), x, mPlotArea.bottom());
  }
  //Fill the rest of the color transfer function with either max or min color
  int areaHeight = mPlotArea.bottom() - mPlotArea.top();
  int halfAreaTop = mPlotArea.top() + areaHeight / 4;
  int halfAreaBottom = mPlotArea.bottom() - areaHeight / 4;
  if (areaLeft > mPlotArea.left())
  {
    for (int x = mPlotArea.left(); x < areaLeft; x++)
    {
      double* rgb = trFunc->GetColor(mImage->getMin());
      painter.setPen(QColor(int(rgb[0]*255), int(rgb[1]*255), int(rgb[2]*255)));
      painter.drawLine(x, halfAreaTop, x, halfAreaBottom);
    }
  }
  if (areaRight < mPlotArea.right())
   {
     for (int x = areaRight; x < mPlotArea.right(); x++)
     {
       double* rgb = trFunc->GetColor(mImage->getMax());
       painter.setPen(QColor(int(rgb[0]*255), int(rgb[1]*255), int(rgb[2]*255)));
       painter.drawLine(x, halfAreaTop, x, halfAreaBottom);
     }
   }

  // Go through each point and draw squares

  ssc::ColorMapPtr colorMapPtr = mImageTF->getColorMap();
  QPoint lastScreenPoint;
  this->mPointRects.clear();
  for (ssc::ColorMap::iterator colorPoint = colorMapPtr->begin();
       colorPoint != colorMapPtr->end();
       colorPoint++)
  {
    // Get the screen (plot) position of this point
    QPoint screenPoint = QPoint(
      static_cast<int>(areaLeft + areaWidth *
                       (colorPoint->first - mImage->getMin()) /
                       static_cast<double>(mImage->getRange())),
                       mPlotArea.bottom());

  // Draw the rectangle
  QRect pointRect(screenPoint.x() - mBorder, mFullArea.top(),
                  mBorder*2, mFullArea.height());
  painter.setPen(pointPen);
  painter.drawRect(pointRect);
  this->mPointRects[colorPoint->first] = pointRect;

  // Store the point
  lastScreenPoint = screenPoint;
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
bool TransferFunctionColorWidget::isInsideCurrentPoint()
{
	mEndPoint = false;
  std::map<int, QRect>::iterator it = mPointRects.begin();
  for(;it != mPointRects.end(); ++it)
  {
    if (it->second.contains(mCurrentClickX, mCurrentClickY))
    {
      mCurrentPoint.position = it->first;
			if (it == mPointRects.begin() || it == --mPointRects.end())
				mEndPoint = true;
			ssc::ColorMapPtr colorMapPtr = mImageTF->getColorMap();
			if (colorMapPtr->find(mCurrentPoint.position) != colorMapPtr->end())
			  mCurrentPoint.value = colorMapPtr->find(mCurrentPoint.position)->second;
      return true;
    }
  }
  mCurrentPoint.reset();
  return false;
}
void TransferFunctionColorWidget::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu;
	
	//TODO: Fix crash at this position
  menu.addAction(mCustomColorAction);

  if (isInsideCurrentPoint() && !mEndPoint)
  {
    menu.addSeparator();
    menu.addAction(mRemoveColorAction);
  }
  menu.exec(event->globalPos());
  
  /*for(int i = 0; i < CustusSceneManager::instance().numColors(); i++)
  {
    const unsigned char* c = CustusSceneManager::instance().getColor(i).color;
    int id = cmenu.insertItem(makeIcon(QColor(c[0], c[1], c[2])),
                              CustusSceneManager::instance().getColor(i).name,
                              this, SLOT(setColor(int)));
    cmenu.setItemParameter(id, i);
    if(colorindex_selected_ == i) cmenu.setItemChecked(id, true);
  }
  cmenu.insertSeparator();
  int id = cmenu.insertItem("Custom color...", this, SLOT(setColor(int)));
  cmenu.setItemParameter(id, CustusSceneManager::instance().numColors());
  if(colorindex_selected_ == CustusSceneManager::instance().numColors())
    cmenu.setItemChecked(id, true);
  
  popupindex_ = isInsidePoint(mCurrentClickX, mCurrentClickY);
  if(popupindex_ != -1)
  {
    if((popupindex_ > 0) && (popupindex_ < transfx_->getMaxValue()))
    {
      cmenu.insertSeparator();
      id = cmenu.insertItem("Remove point", this, SLOT(removePoint()));
    }
    cmenu.exec(mapToGlobal(QPoint(mCurrentClickX, mCurrentClickY)));
  }*/
}
  
TransferFunctionColorWidget::ColorPoint TransferFunctionColorWidget::getCurrentColorPoint()
{
  ColorPoint point;
  point.position = calculateXPositionInTrFunc(mCurrentClickX);
  point.position = ssc::constrainValue(point.position, mImage->getMin(), mImage->getMax());

	// Use vtkColorTransferFunction for interpolation
//	vtkColorTransferFunctionPtr trFunc = mImageTF->getColorTF();
  vtkColorTransferFunctionPtr trFunc = vtkColorTransferFunctionPtr::New();
  mImageTF->fillColorTFFromMap(trFunc);


	double* rgb = trFunc->GetColor(point.position);
  point.value = QColor(int(rgb[0]*255), int(rgb[1]*255), int(rgb[2]*255));

  return point;
}

void TransferFunctionColorWidget::moveCurrentPoint()
{
  if(!mCurrentPoint.isValid())
    return;

  ColorPoint newColorPoint = this->getCurrentColorPoint();
	newColorPoint.value = mCurrentPoint.value;

//  ssc::ImageTF3DPtr transferFunction = mCurrentImage->getTransferFunctions3D();

  ssc::ColorMap::iterator prevPointIterator = mImageTF->getColorMap()->find(mCurrentPoint.position);

  if (mCurrentPoint.position != mImage->getMin()
    && mCurrentPoint.position != mImage->getMax() )
  {
    ssc::ColorMap::iterator nextPointIterator = prevPointIterator;
    prevPointIterator--;
    nextPointIterator++;

    if (newColorPoint.position <= prevPointIterator->first)
      newColorPoint.position = prevPointIterator->first + 1;
    else if (newColorPoint.position >= nextPointIterator->first)
      newColorPoint.position = nextPointIterator->first - 1;

    mImageTF->removeColorPoint(mCurrentPoint.position);
    mImageTF->addColorPoint(newColorPoint.position, newColorPoint.value);
    mCurrentPoint = newColorPoint;
  }
	this->update();
}
  
void TransferFunctionColorWidget::setColorSlot()
{
//  setColorSlotDelayed(); // crashed sporadically
  QTimer::singleShot(1, this, SLOT(setColorSlotDelayed()));
}


void TransferFunctionColorWidget::setColorSlotDelayed()
{
   ColorPoint newPoint = mCurrentPoint;
	if (!newPoint.isValid())
		newPoint = getCurrentColorPoint();

  QColor result = QColorDialog::getColor( newPoint.value, this);

  if (result.isValid() && result != newPoint.value)
  {
    ssc::ColorMapPtr colorMapPtr = mImageTF->getColorMap();

    // Check if the point is already in the map
    ssc::ColorMap::iterator pointIterator = colorMapPtr->find(newPoint.position);

//    if (pointIterator != colorMapPtr->end())
//      mImageTF->setColorValue(newPoint.position, result);
//    else
    mImageTF->addColorPoint(newPoint.position, result);
    newPoint.value = result;
  }
  // TODO: update /render() ???
  this->update();
}
void TransferFunctionColorWidget::removeColorSlot()
{
  if(!this->mCurrentPoint.isValid())
    return;

  mImageTF->removeColorPoint(this->mCurrentPoint.position);

  // TODO: call update/render???
  this->update();
  //GUIManager::instance().getRenderWidget()->render();
}
}//namespace cx
