/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKPADWIDGET_H_
#define CXTRACKPADWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include <vector>
#include <QtWidgets>

#include <QGraphicsView>
#include "cxVector3D.h"
#include "cxDoubleWidgets.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxMousePadWidget.h"

namespace cx
{

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
class cxGui_EXPORT TrackPadWidget : public BaseWidget
{
  Q_OBJECT

public:
  TrackPadWidget(ViewServicePtr viewService, QWidget* parent);
  virtual ~TrackPadWidget();

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
  ViewServicePtr mViewService;

  vtkCameraPtr getCamera() const;
  void defineRotateLayout();
  void definePanLayout();

  void createStandard3DViewActions();
};

}//end namespace cx



#endif /* CXTRACKPADWIDGET_H_ */
