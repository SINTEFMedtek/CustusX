/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
