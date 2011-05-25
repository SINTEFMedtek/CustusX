#ifndef CXTRACKPADWIDGET_H_
#define CXTRACKPADWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include <QGraphicsView>
#include "sscVector3D.h"
#include "sscDoubleWidgets.h"
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class CameraControl> CameraControlPtr;

/**
 * Internal class for the MousePadWidget
 */
class MousePadWidgetInternal : public QFrame
{
  Q_OBJECT

public:
  MousePadWidgetInternal(QWidget* parent, QSize minimumSize);
  virtual ~MousePadWidgetInternal();
  void setFixedXPos(bool on);
  void setFixedYPos(bool on);
  virtual QSize minimumSizeHint() const { return mMinSize; }
signals:
  void mouseMoved(QPointF deltaN);
protected:
  void paintEvent(QPaintEvent* event);
  virtual void showEvent (QShowEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void resizeEvent(QResizeEvent* event);

private:
  QPoint mLastPos;
  bool mFixPosX;
  bool mFixPosY;
  QSize mMinSize;
  void fixPos();
};

/**
 * A touchpad-friendly area  for performing 1D/2D scroll operations.
 */
class MousePadWidget : public QFrame
{
  Q_OBJECT

public:
  MousePadWidget(QWidget* parent, QSize minimumSize);
  virtual ~MousePadWidget();
  void setFixedXPos(bool on);
  void setFixedYPos(bool on);
signals:
  void mouseMoved(QPointF deltaN);
protected:
private:
  class MousePadWidgetInternal* mInternal;
};

/**
 * \class TrackPadWidget
 *
 * \brief Widget for controlling the camera in the 3D view.
 *  This widget is designed for use on a touchpad device.
 *  Example is to use iPad/iPhone as a secondary screen with this widget on.
 *
 * \date Oct 7, 2010
 * \author Christian Askeland, SINTEF
 */

class TrackPadWidget : public BaseWidget
{
  Q_OBJECT

public:
  TrackPadWidget(QWidget* parent);
  virtual ~TrackPadWidget();

  virtual QString defaultWhatsThis() const;

signals:

protected slots:
  void dollySlot(QPointF delta);
  void panXZSlot(QPointF delta);
  void rotateYSlot(QPointF delta);
  void rotateXZSlot(QPointF delta);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  CameraControlPtr mCameraControl;
  QVBoxLayout* mTopLayout;
  QSize mMinPadSize;
  QSize mMinBarSize;

  vtkCameraPtr getCamera() const;
  void defineRotateLayout();
  void definePanLayout();

  void createStandard3DViewActions();
};

}//end namespace cx



#endif /* CXTRACKPADWIDGET_H_ */
