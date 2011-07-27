/*
 * sscMousePadWidget.h
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#ifndef SSCMOUSEPADWIDGET_H_
#define SSCMOUSEPADWIDGET_H_

#include <QFrame>

namespace ssc
{

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

}

#endif /* SSCMOUSEPADWIDGET_H_ */
