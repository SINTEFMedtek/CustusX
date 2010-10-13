/*
 * cxCameraControlWidget.h
 *
 *  Created on: Oct 7, 2010
 *      Author: christiana
 */

#ifndef CXCAMERACONTROLWIDGET_H_
#define CXCAMERACONTROLWIDGET_H_

#include <vector>
#include <QtGui>
#include <QGraphicsView>
#include "sscVector3D.h"
#include "sscDoubleWidgets.h"
#include "sscForwardDeclarations.h"
#include "vtkSmartPointer.h"

typedef vtkSmartPointer<class vtkCamera> vtkCameraPtr;


namespace cx
{

class MousePadWidget : public QFrame
{
  Q_OBJECT

public:
  MousePadWidget(QWidget* parent, QSize minimumSize);
  virtual ~MousePadWidget();
  void setFixedXPos(bool on);
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
  QSize mMinSize;
  void fixPos();
};

/**
 */
class CameraControlWidget : public QWidget
{
  Q_OBJECT

public:
  CameraControlWidget(QWidget* parent);
  virtual ~CameraControlWidget();

signals:

protected slots:
  void dollySlot(QPointF delta);
  void panXZSlot(QPointF delta);
  void rotateYSlot(QPointF delta);
  void rotateXZSlot(QPointF delta);
  void setStandard3DViewActionSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  QVBoxLayout* mTopLayout;
  QSize mMinPadSize;
  QSize mMinBarSize;

  vtkCameraPtr getCamera() const;
  void defineRotateLayout();
  void definePanLayout();

  void createStandard3DViewActions();
  QAction* addStandard3DViewAction(QString caption, QString help, ssc::Vector3D viewDirection, QActionGroup* group);
};

}//end namespace cx



#endif /* CXCAMERACONTROLWIDGET_H_ */
