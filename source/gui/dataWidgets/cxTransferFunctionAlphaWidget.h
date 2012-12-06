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

  void setData(ssc::ImagePtr image, ssc::ImageTFDataPtr tfData);
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

  bool isInsideCurrentPoint(int mouseX, int mouseY);///< Checks if a screen coordinate is inside any of the point rectangles
  AlphaPoint getCurrentAlphaPoint(int mouseX, int mouseY);///< Get aplha point based on mCurrentClickX and mCurrentClickY
  void toggleCurrentPoint(int mouseX, int mouseY);///< Turn a transfer function point on or off (depending on it is on or not)
  void moveCurrentAlphaPoint(int mouseX, int mouseY);///< Move the currently selected point to the selected screen coordinate (mCurrentClickX and mCurrentClickY)

  QRect mFullArea; ///< The full widget area.
  QRect mPlotArea; ///< The plot area.
  std::map<int, QRect> mPointRects; ///< Cache with all point rectangles.
  AlphaPoint mCurrentAlphaPoint;///< The current alpha point
	bool mEndPoint;///< Current alpha point is an endpoint

  int mBorder;///< The size of the border around the transferfunction. The size of the rectangles are mBorder * 2

  ssc::ImagePtr mImage;
  ssc::ImageTFDataPtr mImageTF;
  
  bool mReadOnly;///< Is class readOnly? Eg no mouse interaction possible

  virtual QSize sizeHint () const { return QSize(200, 100);};///< Define a recommended size
  ActiveImageProxyPtr mActiveImageProxy;
};
}
#endif /* CXTRANSFERFUNCTIONALPHAWIDGET_H_ */
