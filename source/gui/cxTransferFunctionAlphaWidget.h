#ifndef CXTRANSFERFUNCTIONALPHAWIDGET_H_
#define CXTRANSFERFUNCTIONALPHAWIDGET_H_

#include <QWidget>

#include <map.h>
#include "sscImage.h"

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

public slots:
  void currentImageChangedSlot(ssc::ImagePtr currentImage); ///< listens to the contextdockwidget for when the current image is changed

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

  virtual void paintEvent(QPaintEvent* event); ///<Reimplemented from superclass.
  virtual void resizeEvent(QResizeEvent* evt);///<Reimplemented from superclass

  bool isInsideCurrentPoint();
  AlphaPoint getCurrentAlphaPoint();///<
  void toggleCurrentPoint(); ///<
  void moveCurrentAlphaPoint(); ///<

  QRect mFullArea; ///< The full widget area.
  QRect mPlotArea; ///< The plot area.
  std::map<int, QRect> mPointRects; ///< Cache with all point rectangles.
  AlphaPoint mCurrentAlphaPoint;
/*
  int mCurrentAlphaValue; ///<The current value  - used to update the info widget.
  int mCurrentAlphaPosition; ///<The currently selected scalar.
*/
  int mCurrentClickX, mCurrentClickY;

  ssc::ImagePtr mCurrentImage;
};
}
#endif /* CXTRANSFERFUNCTIONALPHAWIDGET_H_ */
