#ifndef CXTRANSFERFUNCTIONALPHAWIDGET_H_
#define CXTRANSFERFUNCTIONALPHAWIDGET_H_

#include <QWidget>

#include <sscImage.h>

class QRect;

namespace cx
{
/**
 * \class TransferFunctionAlphaWidget
 *
 * \brief
 *
 * \date Mar 23, 2009
 * \author: Janne Beate Bakeng, SINTEF
 * \author: Ole Vegard Solberg, SINTEF
 */
class TransferFunctionAlphaWidget : public QWidget
{    
  Q_OBJECT
  
public:
  TransferFunctionAlphaWidget(QWidget* parent);
  ~TransferFunctionAlphaWidget();

signals:
  void positionChanged(int);///< Emits this signal whenever the mouse is moved inside the widget
  
public slots:
  //void currentImageChangedSlot(ssc::ImagePtr currentImage); ///< listens to the contextdockwidget for when the current image is changed
  void activeImageChangedSlot(); ///< listens to the contextdockwidget for when the current image is changed

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

  bool isInsideCurrentPoint();///< Checks if a screen coordinate is inside any of the point rectangles
  AlphaPoint getCurrentAlphaPoint();///< Get aplha point based on mCurrentClickX and mCurrentClickY
  void toggleCurrentPoint();///< Turn a transfer function point on or off (depending on it is on or not)
  void moveCurrentAlphaPoint();///< Move the currently selected point to the selected screen coordinate (mCurrentClickX and mCurrentClickY)

  QRect mFullArea; ///< The full widget area.
  QRect mPlotArea; ///< The plot area.
  std::map<int, QRect> mPointRects; ///< Cache with all point rectangles.
  AlphaPoint mCurrentAlphaPoint;///< The current alpha point
	bool mEndPoint;///< Current alpha point is an endpoint

  int mCurrentClickX, mCurrentClickY;///< The x, y coordinate currently selected with the mouse

  int mBorder;///< The size of the border around the transferfunction. The size of the rectangles are mBorder * 2

  ssc::ImagePtr mCurrentImage;
  
  virtual QSize sizeHint () const { return QSize(200, 100);};///< Define a recommended size
};
}
#endif /* CXTRANSFERFUNCTIONALPHAWIDGET_H_ */
