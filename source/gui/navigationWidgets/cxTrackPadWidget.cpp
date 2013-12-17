/*
 * cxTrackPadWidget.cpp
 *
 *  \date Oct 7, 2010
 *      \author christiana
 */

#include "cxTrackPadWidget.h"

#include <QVBoxLayout>
#include <QScrollBar>
#include <QTouchEvent>

#include <QAction>
#include <QRadialGradient>
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"
#include "cxViewManager.h"
#include "cxDataInterface.h"
#include "cxCameraControl.h"
#include "sscBoundingBox3D.h"
#include "sscView.h"

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

  mTopLayout = new QVBoxLayout(this);

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
  camera->OrthogonalizeViewUp(); // needed when using azimuth, according to docs (failure to do this causes strange zooming effects)
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
  Vector3D position(camera->GetPosition());
  Vector3D focus(camera->GetFocalPoint());
  Vector3D vup(camera->GetViewUp());

  Vector3D e_x = cross(focus-position, vup).normal();
  Vector3D e_y = vup.normal();

  DoubleBoundingBox3D bb(viewManager()->get3DView()->getRenderer()->ComputeVisiblePropBounds());

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
