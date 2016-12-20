/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXTRANSFERFUNCTIONCOLORWIDGET_H_
#define CXTRANSFERFUNCTIONCOLORWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include "cxImage.h"
#include "cxForwardDeclarations.h"
#include "cxActiveImageProxy.h"

class QRect;
class QMenu;

namespace cx
{
/**
 * \class TransferFunctionColorWidget
 *
 * \brief
 * \ingroup cx_gui
 *
 * \date Apr 01, 2009
 * \\author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT TransferFunctionColorWidget : public BaseWidget
{
  Q_OBJECT
  
public:
  TransferFunctionColorWidget(ActiveDataPtr activeData, QWidget* parent);
  virtual ~TransferFunctionColorWidget();
  
  void setData(ImagePtr image, ImageTFDataPtr tfData);

public slots:
  void activeImageTransferFunctionsChangedSlot(); ///< Acts when the image's transfer function is changed
  
protected slots:
  void removeColorSlot();
  void setColorSlot();
  void setColorSlotDelayed();

protected:
  struct ColorPoint
  {
	  int intensity;
	  QColor value;
	  ColorPoint() : intensity(-1000000), value(255, 255, 255) {}
	  void reset()
	  {
		  intensity = -1000000;
		  value = QColor(255, 255, 255);
	  }
	  bool isValid()
	  {
		  if(intensity != -1000000)
			  return true;
		  return false;
	  }
  }; ///< Internal placeholder for a color point
  
  QAction* mCustomColorAction;///< Action for choosing custom color
  QAction* mRemoveColorAction;///< Action for removing a color
      
  QRect mFullArea;///< The full widget area
  QRect mPlotArea;///< The plot area 
  int mBorder;///< The size of the border around the transferfunction. The size of the rectangles are mBorder * 2

  std::map<int, QRect> mPointRects;///< Cache with all point rectangles  
  ColorPoint mSelectedPoint;///< The currently selected point
//  int mCurrentClickX, mCurrentClickY;///< The x coordinate currently selected with the mouse
  QPoint mCurrentClickPos;

  ColorPoint selectPoint(QPoint pos) const;
  std::pair<int,int> findAllowedMoveRangeAroundColorPoint(int selectedPointIntensity);
  void paintColorPointsAndGenerateCache(QPainter& painter);
  void paintColorBar(QPainter& painter);
  void updateTooltip(QPoint pos);
  void updateTooltip(ColorPoint point);

  ImagePtr mImage;
  ImageTFDataPtr mImageTF;
  ActiveImageProxyPtr mActiveImageProxy;
  
  bool isEndpoint(int intensity) const;
//  bool isInsideCurrentPoint();///< Checks if a screen coordinate is inside any of the point rectangles. Sets mCurrentPoint.position and mCurrentPoint.value
  void contextMenuEvent(QContextMenuEvent *event);///< Decides what happens when you rightclick in a view
  
    
  QPoint getPlotPoint(int iScalar);///< Calculate the screen point of a given scalar (x-axis) and value (y-axis)
  ColorPoint getCurrentColorPoint();///< Calculate the color point (position and color) based on clicked x coordinate
  ColorPoint getCurrentColorPoint(int clickX);
//  void moveCurrentPoint();///< Move the currently selected point to the selected screen coordinate (mCurrentClickX)
  void moveSelectedPointTo(ColorPoint newPoint);

  virtual void enterEvent(QEvent* event);
  virtual void leaveEvent(QEvent* event);
  virtual void mousePressEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseReleaseEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseMoveEvent(QMouseEvent* event);  ///< Reimplemented from superclass
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void resizeEvent(QResizeEvent* evt);///< Reimplemented from superclass
  virtual void paintEvent(QPaintEvent* event); ///< Reimplemented from superclass. Paints the transferfunction GUI
  
  virtual QSize sizeHint () const { return QSize(100, 30);};///< Define a recommended size

  int screenX2imageIntensity(int screenX);///< Calculate the correct position within the color transfer function, based on a screen x coordinate
  int imageIntensity2screenX(int intensity);
};

}
#endif /* CXTRANSFERFUNCTIONCOLORWIDGET_H_ */
