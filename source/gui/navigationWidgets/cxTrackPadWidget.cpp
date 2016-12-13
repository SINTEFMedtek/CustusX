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
#include "cxTrackPadWidget.h"

#include <QVBoxLayout>
#include <QScrollBar>
#include <QTouchEvent>

#include <QAction>
#include <QRadialGradient>
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"
#include "cxDataInterface.h"
#include "cxCameraControl.h"
#include "cxBoundingBox3D.h"
#include "cxView.h"
#include "cxViewService.h"

namespace cx
{

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

TrackPadWidget::TrackPadWidget(ViewServicePtr viewService, QWidget* parent) :
	BaseWidget(parent, "track_pad_widget", "Camera Control"),
	mViewService(viewService)
{
	this->setToolTip("Track pad camera control");
  mCameraControl = viewService->getCameraControl();

  mMinPadSize = QSize(50,50);
  mMinBarSize = QSize(20,50);

  mTopLayout = new QVBoxLayout(this);

  this->createStandard3DViewActions();
  this->definePanLayout();
  this->defineRotateLayout();
}

void TrackPadWidget::createStandard3DViewActions()
{
  QActionGroup* group = mCameraControl->createStandard3DViewActions();

  QToolBar* toolBar = new QToolBar(this);
  mTopLayout->addWidget(toolBar);
  toolBar->addActions(group->actions());
  toolBar->addSeparator();
}

void TrackPadWidget::defineRotateLayout()
{
  QGroupBox* group = new QGroupBox("rotate", this);
  group->setFlat(true);
  mTopLayout->addWidget(group);

  QHBoxLayout* layout = new QHBoxLayout;
  layout->setMargin(4);
  group->setLayout(layout);

  MousePadWidget* rotateWidget = new MousePadWidget(this, mMinPadSize);
  connect(rotateWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(rotateXZSlot(QPointF)));
  layout->addWidget(rotateWidget, 4);

  MousePadWidget* rotateYWidget = new MousePadWidget(this, mMinBarSize);
  rotateYWidget->setFixedXPos(true);
  connect(rotateYWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(rotateYSlot(QPointF)));
  layout->addWidget(rotateYWidget, 1);
}


void TrackPadWidget::definePanLayout()
{
  QGroupBox* group = new QGroupBox("pan", this);
  group->setFlat(true);
  mTopLayout->addWidget(group);

  QHBoxLayout* panLayout = new QHBoxLayout;
  panLayout->setMargin(4);
  group->setLayout(panLayout);

  MousePadWidget* panWidget = new MousePadWidget(this, mMinPadSize);
  connect(panWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(panXZSlot(QPointF)));
  panLayout->addWidget(panWidget, 4);

  MousePadWidget* dollyWidget = new MousePadWidget(this, mMinBarSize);
  dollyWidget->setFixedXPos(true);
  connect(dollyWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(dollySlot(QPointF)));
  panLayout->addWidget(dollyWidget, 1);
}

vtkCameraPtr TrackPadWidget::getCamera() const
{
  return mViewService->get3DView()->getRenderer()->GetActiveCamera();
}

void TrackPadWidget::rotateYSlot(QPointF delta)
{
  double scale = 180;
  double factor = scale * delta.y();

  this->getCamera()->Roll(factor);
}

void TrackPadWidget::rotateXZSlot(QPointF delta)
{
  vtkCameraPtr camera = this->getCamera();
  double scale = 180;

  camera->Azimuth(-scale * delta.x());
  camera->Elevation(scale * delta.y());
  camera->OrthogonalizeViewUp(); // needed when using azimuth, according to docs (failure to do this causes strange zooming effects)
}

void TrackPadWidget::dollySlot(QPointF delta)
{
  double factor = 1 + delta.y();
  this->getCamera()->Dolly(factor);
  mViewService->get3DView()->getRenderer()->ResetCameraClippingRange();
}

void TrackPadWidget::panXZSlot(QPointF delta)
{
  vtkCameraPtr camera = this->getCamera();
  Vector3D position(camera->GetPosition());
  Vector3D focus(camera->GetFocalPoint());
  Vector3D vup(camera->GetViewUp());

  Vector3D e_x = cross(focus-position, vup).normal();
  Vector3D e_y = vup.normal();

  DoubleBoundingBox3D bb(mViewService->get3DView()->getRenderer()->ComputeVisiblePropBounds());

  double volSize = bb.range().length() / pow(3, 1.0/3.0); // mm size of volume
  double scale = volSize;///padSize;
  Vector3D t = scale * (-delta.x() * e_x + delta.y() * e_y);

  position += t;
  focus += t;

  camera->SetPosition(position.begin());
  camera->SetFocalPoint(focus.begin());
}

TrackPadWidget::~TrackPadWidget()
{
}

void TrackPadWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void TrackPadWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx
