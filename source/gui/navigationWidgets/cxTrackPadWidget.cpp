/*
 * cxTrackPadWidget.cpp
 *
 *  Created on: Oct 7, 2010
 *      Author: christiana
 */

#include "cxTrackPadWidget.h"

#include <QVBoxLayout>
#include <QScrollBar>
#include <QTouchEvent>

#include <QToolButton>
#include <QAction>
#include <QRadialGradient>
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"
#include "cxViewManager.h"
#include "cxView3D.h"
#include "cxDataInterface.h"
#include "cxCameraControl.h"

namespace cx
{

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

TrackPadWidget::TrackPadWidget(QWidget* parent) :
    BaseWidget(parent, "TrackPadWidget", "Camera Control")
{
  mCameraControl.reset(new CameraControl());

  mMinPadSize = QSize(50,50);
  mMinBarSize = QSize(20,50);

  //layout
  mTopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  this->createStandard3DViewActions();
  this->definePanLayout();
  this->defineRotateLayout();
}

QString TrackPadWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Trackpad for touch screen devices</h3>"
      "<p>Helps the user control the camera on a touch screen.</p>"
      "<p><i></i></p>"
      "</html>";
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

  ssc::MousePadWidget* rotateWidget = new ssc::MousePadWidget(this, mMinPadSize);
//  rotateWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//  rotateWidget->setLineWidth(3);
  connect(rotateWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(rotateXZSlot(QPointF)));
  layout->addWidget(rotateWidget, 4);

  ssc::MousePadWidget* rotateYWidget = new ssc::MousePadWidget(this, mMinBarSize);
  rotateYWidget->setFixedXPos(true);
//  rotateYWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//  rotateYWidget->setLineWidth(3);
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

  ssc::MousePadWidget* panWidget = new ssc::MousePadWidget(this, mMinPadSize);
//  panWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//  panWidget->setLineWidth(3);
  connect(panWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(panXZSlot(QPointF)));
  panLayout->addWidget(panWidget, 4);

  ssc::MousePadWidget* dollyWidget = new ssc::MousePadWidget(this, mMinBarSize);
  dollyWidget->setFixedXPos(true);
//  dollyWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//  dollyWidget->setLineWidth(3);
  connect(dollyWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(dollySlot(QPointF)));
  panLayout->addWidget(dollyWidget, 1);
}

vtkCameraPtr TrackPadWidget::getCamera() const
{
  return viewManager()->get3DView()->getRenderer()->GetActiveCamera();
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
}

void TrackPadWidget::dollySlot(QPointF delta)
{
  double factor = 1 + delta.y();
  this->getCamera()->Dolly(factor);
  viewManager()->get3DView()->getRenderer()->ResetCameraClippingRange();
}

void TrackPadWidget::panXZSlot(QPointF delta)
{
  vtkCameraPtr camera = this->getCamera();
  ssc::Vector3D position(camera->GetPosition());
  ssc::Vector3D focus(camera->GetFocalPoint());
  ssc::Vector3D vup(camera->GetViewUp());

  ssc::Vector3D e_x = ssc::cross(focus-position, vup).normal();
  ssc::Vector3D e_y = vup.normal();

  ssc::DoubleBoundingBox3D bb(viewManager()->get3DView()->getRenderer()->ComputeVisiblePropBounds());

  double volSize = bb.range().length() / pow(3, 1.0/3.0); // mm size of volume
  double scale = volSize;///padSize;
  ssc::Vector3D t = scale * (-delta.x() * e_x + delta.y() * e_y);

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
