#ifndef CXTRANSFERFUNCTIONCOLORWIDGET_H_
#define CXTRANSFERFUNCTIONCOLORWIDGET_H_

#include "cxBaseWidget.h"

#include <sscImage.h>
#include "sscForwardDeclarations.h"
#include "cxActiveImageProxy.h"

class QRect;
class QMenu;

namespace cx
{
/**
 * \class TransferFunctionColorWidget
 *
 * \brief
 * \ingroup cxGUI
 *
 * \date Apr 01, 2009
 * \\author Ole Vegard Solberg, SINTEF
 */
class TransferFunctionColorWidget : public BaseWidget
{
  Q_OBJECT
  
public:
  TransferFunctionColorWidget(QWidget* parent);
  virtual ~TransferFunctionColorWidget();
  virtual QString defaultWhatsThis() const;
  
  void setData(ssc::ImagePtr image, ssc::ImageTFDataPtr tfData);

public slots:
  void activeImageTransferFunctionsChangedSlot(); ///< Acts when the image's transfer function is changed
  
protected slots:
  void removeColorSlot();
  void setColorSlot();
  void setColorSlotDelayed();

protected:
  struct ColorPoint
  {
    int position;
    QColor value;
    ColorPoint() :
    position(-1000000),
    value(255, 255, 255)
    {}
    void reset()
    {
      position = -1000000;
      value = QColor(255, 255, 255);
    }
    bool isValid()
    {
      if(position != -1000000)
        return true;
      return false;
    }
  }; ///< Internal placeholder for a color point
  
  QAction* mCustomColorAction;///< Action for choosing custom color
  QAction* mRemoveColorAction;///< Action for removing a color
      
  QRect mFullArea;///< The full widget area
  QRect mPlotArea;///< The plot area 
  
  std::map<int, QRect> mPointRects;///< Cache with all point rectangles
  
  ColorPoint mCurrentPoint;///< The currently selected point
	bool mEndPoint;///< Current point is an endpoint
  int mColorindexSelected;
  int mCurrentClickX, mCurrentClickY;///< The x coordinate currently selected with the mouse
  int mBorder;///< The size of the border around the transferfunction. The size of the rectangles are mBorder * 2
  
  ssc::ImagePtr mImage;
  ssc::ImageTFDataPtr mImageTF;
  ActiveImageProxyPtr mActiveImageProxy;
  
  bool isInsideCurrentPoint();///< Checks if a screen coordinate is inside any of the point rectangles. Sets mCurrentPoint.position and mCurrentPoint.value
  void contextMenuEvent(QContextMenuEvent *event);///< Decides what happens when you rightclick in a view
  
    
  QPoint getPlotPoint(int iScalar);///< Calculate the screen point of a given scalar (x-axis) and value (y-axis)
  ColorPoint getCurrentColorPoint();///< Calculate the color point (position and color) based on clicked x coordinate
  void moveCurrentPoint();///< Move the currently selected point to the selected screen coordinate (mCurrentClickX)
  
  virtual void mousePressEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseReleaseEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseMoveEvent(QMouseEvent* event);  ///< Reimplemented from superclass
  virtual void resizeEvent(QResizeEvent* evt);///< Reimplemented from superclass
  virtual void paintEvent(QPaintEvent* event); ///< Reimplemented from superclass. Paints the transferfunction GUI
  
  virtual QSize sizeHint () const { return QSize(100, 30);};///< Define a recommended size

  void calculateColorTFBoundaries(int &areaLeft, int &areaRight, int &areaWidth);///< Calculate color transfer function boundaries for GUI, based on window and level
  int calculateXPositionInTrFunc(int screenX);///< Calculate the correct position within the color transfer function, based on a screen x coordinate
};
}
#endif /* CXTRANSFERFUNCTIONCOLORWIDGET_H_ */
