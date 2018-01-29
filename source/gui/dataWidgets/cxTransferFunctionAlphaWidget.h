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

#ifndef CXTRANSFERFUNCTIONALPHAWIDGET_H_
#define CXTRANSFERFUNCTIONALPHAWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include <limits.h>
#include "cxImage.h"
#include "cxForwardDeclarations.h"
#include "cxActiveImageProxy.h"
class QRect;

namespace cx
{
/**
 * \class TransferFunctionAlphaWidget
 *
 * \brief
 * \ingroup cx_gui
 *
 * \date Mar 23, 2009
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT TransferFunctionAlphaWidget : public BaseWidget
{    
  Q_OBJECT

	void clearBackground(QPainter &painter);
public:
  TransferFunctionAlphaWidget(ActiveDataPtr activeData, QWidget* parent);
  virtual ~TransferFunctionAlphaWidget();

  void setData(ImagePtr image, ImageTFDataPtr tfData);
  void setReadOnly(bool readOnly);///< Set class readonly: Disable mouse interaction
  
public slots:
  void activeImageTransferFunctionsChangedSlot(); ///< Acts when the image's transfer function is changed

protected:
  struct AlphaPoint
  {
    int position;
    int value;
    AlphaPoint() :
      position(INT_MIN),
      value(INT_MIN)
      {}
	AlphaPoint(int pos, int val) :
	  position(pos),
	  value(val)
	  {}
	void reset()
    {
      position = INT_MIN;
      value = INT_MIN;
    }
    bool isValid()
    {
      if(position != INT_MIN)
        return true;
      return false;
    }
  }; ///< Internal placeholder for a function point

  virtual void enterEvent(QEvent* event);///< Reimplemented from superclass
  virtual void leaveEvent(QEvent* event); ///< Reimplemented from superclass
  virtual void mousePressEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseReleaseEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseMoveEvent(QMouseEvent* event);  ///< Reimplemented from superclass
  virtual void keyPressEvent(QKeyEvent* event);

  virtual void paintEvent(QPaintEvent* event); ///< Reimplemented from superclass. Paints the transferfunction GUI
  virtual void resizeEvent(QResizeEvent* evt);///< Reimplemented from superclass

  AlphaPoint selectPoint(QPoint pos);
  AlphaPoint getCurrentAlphaPoint(QPoint pos);///< Get aplha point based on mCurrentClickX and mCurrentClickY
  void toggleSelectedPoint(QPoint pos);///< Turn a transfer function point on or off (depending on it is on or not)
  void moveCurrentAlphaPoint(AlphaPoint newAlphaPoint);
  QPoint alpha2screen(AlphaPoint pt) const;
  void updateTooltip(QPoint pos);
  void updateTooltip(AlphaPoint point);

  bool isEndpoint(int intensity) const;
  void paintHistogram(QPainter& painter);
  void paintOpacityGraph(QPainter& painter);
  std::pair<int,int> findAllowedMoveRangeAroundAlphaPoint(int selectedPointIntensity);

  QRect mFullArea; ///< The full widget area.
  QRect mPlotArea; ///< The plot area.
  int mBorder;///< The size of the border around the transferfunction. The size of the rectangles are mBorder * 2
  bool mReadOnly;///< Is class readOnly? Eg no mouse interaction possible

  std::map<int, QRect> mPointRects; ///< Cache with all point rectangles.
  AlphaPoint mSelectedAlphaPoint;///< The current alpha point
//  bool mEndPoint;///< Current alpha point is an endpoint

  ImagePtr mImage;
  ImageTFDataPtr mImageTF;
  ActiveImageProxyPtr mActiveImageProxy;

  virtual QSize sizeHint () const { return QSize(200, 100);};///< Define a recommended size
};
}
#endif /* CXTRANSFERFUNCTIONALPHAWIDGET_H_ */
