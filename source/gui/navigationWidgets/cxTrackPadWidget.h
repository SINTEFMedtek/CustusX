#ifndef CXTRACKPADWIDGET_H_
#define CXTRACKPADWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include <QGraphicsView>
#include "cxVector3D.h"
#include "cxDoubleWidgets.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxMousePadWidget.h"

namespace cx
{

typedef boost::shared_ptr<class CameraControl> CameraControlPtr;



/**
 * \class TrackPadWidget
 *
 * \brief Widget for controlling the camera in the 3D view.
 * \ingroup cx_gui
 *
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
