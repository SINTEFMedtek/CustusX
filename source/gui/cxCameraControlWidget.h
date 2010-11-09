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
  MousePadWidget(QWidget* parent);
  virtual ~MousePadWidget();
signals:
  void mouseMoved(QPoint delta);
protected:
  void paintEvent(QPaintEvent* event);
  virtual void showEvent (QShowEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
private:
  QPoint mLastPos;
};


class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QGraphicsScene *scene = 0, QWidget *parent = 0);

    bool viewportEvent(QEvent *event);

private:
    qreal totalScaleFactor;
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
  void dollySlot();
  void panSlot(QPoint delta);
  void rotateYSlot();
  void rotateXZSlot(QPoint delta);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff
//  virtual bool viewportEvent(QEvent *event);

private:
  MousePadWidget* mPanWidget;
  MousePadWidget* mRotateWidget;
  QVBoxLayout* mTopLayout;

  vtkCameraPtr getCamera() const;
  void defineRotateLayout();
  void definePanLayout();
};

}//end namespace cx



#endif /* CXCAMERACONTROLWIDGET_H_ */
