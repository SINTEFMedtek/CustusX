#ifndef CXTRANSFERFUNCTIONALPHAWIDGET_H_
#define CXTRANSFERFUNCTIONALPHAWIDGET_H_

#include "cxBaseWidget.h"

#include <limits.h>
#include <sscImage.h>
#include "sscForwardDeclarations.h"
#include "cxActiveImageProxy.h"

class QRect;

namespace cx
{
/**
 * \class TransferFunctionAlphaWidget
 *
 * \brief
 * \ingroup cxGUI
 *
 * \date Mar 23, 2009
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Ole Vegard Solberg, SINTEF
 */
class TransferFunctionAlphaWidget : public BaseWidget
{    
  Q_OBJECT
  
public:
  TransferFunctionAlphaWidget(QWidget* parent);
  virtual ~TransferFunctionAlphaWidget();
  virtual QString defaultWhatsThis() const;

  void setData(ImagePtr image, ImageTFDataPtr tfData);
  void setReadOnly(bool readOnly);///< Set class readonly: Disable mouse interaction

signals:
  void positionChanged(int);///< Emits this signal whenever the mouse is moved inside the widget
  
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

  virtual void paintEvent(QPaintEvent* event); ///< Reimplemented from superclass. Paints the transferfunction GUI
  virtual void resizeEvent(QResizeEvent* evt);///< Reimplemented from superclass

  bool isInsideCurrentPoint(QPoint pos);///< Checks if a screen coordinate is inside any of the point rectangles
  AlphaPoint getCurrentAlphaPoint(QPoint pos);///< Get aplha point based on mCurrentClickX and mCurrentClickY
  void toggleCurrentPoint(QPoint pos);///< Turn a transfer function point on or off (depending on it is on or not)
  void moveCurrentAlphaPoint(QPoint pos);///< Move the currently selected point to the selected screen coordinate (mCurrentClickX and mCurrentClickY)
  QPoint alpha2screen(AlphaPoint pt) const;

  bool isEndpoint(int intensity) const;
  void paintHistogram(QPainter& painter);
  void paintOpacityGraph(QPainter& painter);
  std::pair<int,int> findAllowedMoveRangeAroundAlphaPoint(int selectedPointIntensity);

  QRect mFullArea; ///< The full widget area.
  QRect mPlotArea; ///< The plot area.
  int mBorder;///< The size of the border around the transferfunction. The size of the rectangles are mBorder * 2
  bool mReadOnly;///< Is class readOnly? Eg no mouse interaction possible

  std::map<int, QRect> mPointRects; ///< Cache with all point rectangles.
  AlphaPoint mCurrentAlphaPoint;///< The current alpha point
//  bool mEndPoint;///< Current alpha point is an endpoint

  ImagePtr mImage;
  ImageTFDataPtr mImageTF;
  ActiveImageProxyPtr mActiveImageProxy;

  virtual QSize sizeHint () const { return QSize(200, 100);};///< Define a recommended size
};
}
#endif /* CXTRANSFERFUNCTIONALPHAWIDGET_H_ */
