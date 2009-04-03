#include "cxTransferFunctionColorWidget.h"

#include <limits.h>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QMouseEvent>
#include <QColorDialog>

#include <QMenu>

namespace cx
{
TransferFunctionColorWidget::TransferFunctionColorWidget(QWidget* parent) :
  QWidget(parent),
  mColorindexSelected(0),
  mCurrentClickX(INT_MIN),
  mCurrentClickY(INT_MAX),
  mBorder(5)
{
  mCurrentPoint.reset();
  
  // Create right click menu
  mRightClickMenu = new QMenu(this);
  // Create actions
  mCustomColorAction = new QAction(tr("Custom color..."), mRightClickMenu);
  mRemoveColorAction = new QAction(tr("Remove point"), mRightClickMenu);
  
  connect(mCustomColorAction, SIGNAL(triggered(bool)),
    this, SLOT(setColorSlot()));
  connect(mRemoveColorAction, SIGNAL(triggered()),
    this, SLOT(removeColorSlot()));
}
TransferFunctionColorWidget::~TransferFunctionColorWidget()
{}
void TransferFunctionColorWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if(mCurrentImage == currentImage)
    return;

  mCurrentImage = currentImage;
  //TODO: call update or not ???
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
  // TODO: Check:
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
  this->update();
}
void TransferFunctionColorWidget::mouseMoveEvent(QMouseEvent* event)
{
  QWidget::mouseMoveEvent(event);

  if(event->button() != Qt::NoButton)
  {
    // TODO: Check if this works or if x and y must be input values in moveCurrentAlphaPoint
    // Update current screen point for use in moveCurrentAlphaPoint
    mCurrentClickX = event->x();
    mCurrentClickY = event->y();
    this->moveCurrentPoint();
  }
}
void TransferFunctionColorWidget::paintEvent(QPaintEvent* event)
{
  QWidget::paintEvent(event);

  QPainter painter(this);
  QPen pointPen, pointLinePen;
  
  // Fill with white background color and grey plot area background color
  painter.fillRect(this->mFullArea, QColor(170, 170, 170));
  painter.fillRect(this->mPlotArea, QColor(200, 200, 200));
  
  // Draw color-background
  
  ssc::ImageTF3D transferFunction = mCurrentImage->getTransferFunctions3D();
  
  painter.setPen(QColor(140, 140, 210));
  for (int x = mPlotArea.left(); x <= mPlotArea.right(); ++x)
  {
    int point = static_cast<int>(0.5 + (mCurrentImage->getRange() - 1) *
                                 (x - mPlotArea.left() ) /
                                 static_cast<double>(mPlotArea.width()-1));
    QColor color = transferFunction.getColorValue(point);
    painter.setPen(color);
    painter.drawLine(x, mPlotArea.top(), x, mPlotArea.bottom());
  }
  
  // Go through each point and draw squares and lines
  
  ssc::ColorMapPtr colorMapPtr = mCurrentImage->getTransferFunctions3D().getColorMap();
  QPoint lastScreenPoint;
  this->mPointRects.clear();
  for (ssc::ColorMap::iterator colorPoint = colorMapPtr->begin();
       colorPoint != colorMapPtr->end();
       colorPoint++)
  {
    // Get the screen (plot) position of this point
    QPoint screenPoint = QPoint(
      static_cast<int>(mPlotArea.left() + mPlotArea.width() * 
                       colorPoint->first / 
                       static_cast<double>(mCurrentImage->getAlphaRange())),
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
  std::map<int, QRect>::iterator it = mPointRects.begin();
  for(;it != mPointRects.end(); ++it)
  {
    if (it->second.contains(mCurrentClickX, mCurrentClickY))
    {
      mCurrentPoint.position = it->first;
      return true;
    }
  }
  mCurrentPoint.reset();
  return false;
}
void TransferFunctionColorWidget::contextMenuEvent(QContextMenuEvent *event)
{
  mRightClickMenu->clear();
  mRightClickMenu->addAction(mCustomColorAction);
  
  if (isInsideCurrentPoint())
  {
    mRightClickMenu->addSeparator();
    mRightClickMenu->addAction(mRemoveColorAction);
  }
  mRightClickMenu->exec(event->globalPos());
  
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

  point.position = 
    static_cast<int>( mCurrentImage->getAlphaRange() * 
                     (mCurrentClickX - mPlotArea.left()) / 
                     static_cast<double>(mPlotArea.width()) );
  
  if (point.position > mCurrentImage->getMax())
    point.position = mCurrentImage->getMax();
  else if (point.position < mCurrentImage->getMin())
    point.position = mCurrentImage->getMin();

  ssc::ColorMapPtr colorMapPtr = mCurrentImage->getTransferFunctions()->getColorMap();
  if (colorMapPtr->find(point.position) != colorMapPtr->end())
    point.value = colorMapPtr->find(point.position)->second;

  return point;
}

void TransferFunctionColorWidget::moveCurrentPoint()
{
  if(!mCurrentPoint.isValid())
    return;
  
  ColorPoint newColorPoint = this->getCurrentColorPoint();
  
  ssc::ImageTF3D transferFunction = mCurrentImage->getTransferFunctions3D();
  
  ssc::ColorMap::iterator prevPointIterator = 
    transferFunction.getColorMap()->find(mCurrentPoint.position);
  
  if (mCurrentPoint.position == mCurrentImage->getMin()
    || mCurrentPoint.position == mCurrentImage->getMax() )
    transferFunction.setColorValue(newColorPoint.value);
  else
  {
    ssc::ColorMap::iterator nextPointIterator = prevPointIterator;
    prevPointIterator--;
    nextPointIterator++;
    
    if (newColorPoint.position <= prevPointIterator->first)
      newColorPoint.position = prevPointIterator->first + 1;
    else if (newColorPoint.position >= nextPointIterator->first)
      newColorPoint.position = nextPointIterator->first - 1;
   
    transferFunction.removeColorPoint(newColorPoint.position);
    transferFunction.addColorPoint(newColorPoint.position, newColorPoint.value);
    mCurrentPoint = newColorPoint;
  }
}
  
void TransferFunctionColorWidget::setColorSlot()
{
  QColor result = QColorDialog::getColor( mCurrentPoint.value, this);
  
  if (result.isValid())
  {
    ssc::ColorMapPtr colorMapPtr = mCurrentImage->getTransferFunctions3D().getColorMap();
    
    // Check if the point is already in the map
    ssc::ColorMap::iterator pointIterator = colorMapPtr->find(mCurrentPoint.position);
    
    if (pointIterator != colorMapPtr->end())
      mCurrentImage->getTransferFunctions3D().setColorPoint(mCurrentPoint.position, result);
    else
      mCurrentImage->getTransferFunctions3D().addColorPoint(mCurrentPoint.position, result);
    mCurrentPoint.value = result;
  }
  // TODO: update /render() ???
  this->update();
}
void TransferFunctionColorWidget::removeColorSlot()
{
  if(!this->mCurrentPoint.isValid()) 
    return;
    
  mCurrentImage->getTransferFunctions3D().removeColorPoint(this->mCurrentPoint.value);
  
  // TODO: call update/render??? 
  this->update();
  //GUIManager::instance().getRenderWidget()->render();       
}
}//namespace cx
