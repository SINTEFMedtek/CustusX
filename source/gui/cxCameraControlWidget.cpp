/*
 * cxCameraControlWidget.cpp
 *
 *  Created on: Oct 7, 2010
 *      Author: christiana
 */

#include "cxCameraControlWidget.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QAction>
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"
#include "cxViewManager.h"
#include "cxView3D.h"
#include "cxDataInterface.h"

namespace cx
{

MousePadWidget::MousePadWidget(QWidget* parent) : QFrame(parent)
{
}
MousePadWidget::~MousePadWidget()
{
}
void MousePadWidget::mousePressEvent(QMouseEvent* event)
{
  mLastPos = event->pos();
}
void MousePadWidget::mouseMoveEvent(QMouseEvent* event)
{
  QPoint delta = event->pos() - mLastPos;
  emit mouseMoved(delta);
  mLastPos = event->pos();
}

///** Interface to the tool offset of the dominant tool
// */
//class DoubleDataAdapterCamera3DControl : public ssc::DoubleDataAdapter
//{
//  Q_OBJECT
//public:
//  DoubleDataAdapterCamera3DControl();
//  virtual ~DoubleDataAdapterCamera3DControl() {}
//  virtual QString getValueName() const { return "Zoom"; }
//  virtual double getValue() const;
//  virtual bool setValue(double val);
//  virtual void connectValueSignals(bool on) {}
//  ssc::DoubleRange getValueRange() const;
//
//private slots:
//protected:
//  vtkCameraPtr getCamera() const;
//};
//
//DoubleDataAdapterCamera3DControl::DoubleDataAdapterCamera3DControl()
//{
//}
//
//vtkCameraPtr DoubleDataAdapterCamera3DControl::getCamera() const
//{
//  return viewManager()->get3DView()->GetRenderer()->GetActiveCamera();
//}
//
//double DoubleDataAdapterCamera3DControl::getValue() const
//{
//  if (!this->getCamera())
//    return 1.0;
////  this->getCamera()->();
////  if (mTool)
////    return mTool->getTooltipOffset();
////  return 0.0;
//}
//
//bool DoubleDataAdapterCamera3DControl::setValue(double val)
//{
//  if (!this->getCamera())
//    return false;
//  this->getCamera()->Dolly(1.1);
//  return true;
//}
//
//ssc::DoubleRange DoubleDataAdapterCamera3DControl::getValueRange() const
//{
//  double range = 10;
//  return ssc::DoubleRange(0.1,range,range/1000.0);
//}
//


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------




CameraControlWidget::CameraControlWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("CameraControlWidget");
  this->setWindowTitle("Camera Control");

  //layout
  mTopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  this->definePanLayout();
  this->defineRotateLayout();

  mTopLayout->addStretch();

  //connect(ssc::dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(updateSlot()));
  //updateSlot();
}

void CameraControlWidget::defineRotateLayout()
{
  QHBoxLayout* layout = new QHBoxLayout;
  mTopLayout->addLayout(layout);

  mRotateWidget = new MousePadWidget(this);
  mRotateWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  mRotateWidget->setLineWidth(3);
  connect(mRotateWidget, SIGNAL(mouseMoved(QPoint)), this, SLOT(rotateXZSlot(QPoint)));

  layout->addWidget(mRotateWidget);

  QVBoxLayout* dollyLayout = new QVBoxLayout;
  layout->addLayout(dollyLayout);

  QAction* mDollyInAction = new QAction("RollUp", this);
  mDollyInAction->setData(5);
  connect(mDollyInAction, SIGNAL(triggered()), this, SLOT(rotateYSlot()));
  QToolButton* mDollyInButton = new QToolButton(this);
  mDollyInButton->setArrowType(Qt::UpArrow);
  mDollyInButton->setDefaultAction(mDollyInAction);
  dollyLayout->addWidget(mDollyInButton);

  QAction* mDollyOutAction = new QAction("RollDown", this);
  mDollyOutAction->setData(-5);
  connect(mDollyOutAction, SIGNAL(triggered()), this, SLOT(rotateYSlot()));
  QToolButton* mDollyOutButton = new QToolButton(this);
  mDollyOutButton->setArrowType(Qt::DownArrow);
  mDollyOutButton->setDefaultAction(mDollyOutAction);
  dollyLayout->addWidget(mDollyOutButton);
}


void CameraControlWidget::definePanLayout()
{
  QHBoxLayout* panLayout = new QHBoxLayout;
  mTopLayout->addLayout(panLayout);

  mPanWidget = new MousePadWidget(this);
  mPanWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  mPanWidget->setLineWidth(3);
  connect(mPanWidget, SIGNAL(mouseMoved(QPoint)), this, SLOT(panSlot(QPoint)));

  panLayout->addWidget(mPanWidget);

  QVBoxLayout* dollyLayout = new QVBoxLayout;
  panLayout->addLayout(dollyLayout);

  QAction* mDollyInAction = new QAction("ZIn", this);
  mDollyInAction->setData(1.1);
  connect(mDollyInAction, SIGNAL(triggered()), this, SLOT(dollySlot()));
  QToolButton* mDollyInButton = new QToolButton(this);
  mDollyInButton->setArrowType(Qt::UpArrow);
  mDollyInButton->setDefaultAction(mDollyInAction);
  dollyLayout->addWidget(mDollyInButton);

  QAction* mDollyOutAction = new QAction("ZIn", this);
  mDollyOutAction->setData(0.9);
  connect(mDollyOutAction, SIGNAL(triggered()), this, SLOT(dollySlot()));
  QToolButton* mDollyOutButton = new QToolButton(this);
  mDollyOutButton->setArrowType(Qt::DownArrow);
  mDollyOutButton->setDefaultAction(mDollyOutAction);
  dollyLayout->addWidget(mDollyOutButton);
}

vtkCameraPtr CameraControlWidget::getCamera() const
{
  return viewManager()->get3DView()->getRenderer()->GetActiveCamera();
}

void CameraControlWidget::rotateYSlot()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;
  double factor = action->data().toDouble();
  this->getCamera()->Roll(factor);
}

void CameraControlWidget::rotateXZSlot(QPoint delta)
{
  vtkCameraPtr camera = this->getCamera();
//  ssc::Vector3D position(camera->GetPosition());
//  ssc::Vector3D focus(camera->GetFocalPoint());
//  ssc::Vector3D vup(camera->GetViewUp());
//
//  ssc::Vector3D e_x = cross(focus-position, vup).normal();
//  ssc::Vector3D e_y = vup.normal();
//
//  ssc::DoubleBoundingBox3D bb(viewManager()->get3DView()->getRenderer()->ComputeVisiblePropBounds());

  int padSize = (mRotateWidget->size().width() + mRotateWidget->size().height())/2.0; // pixel size of trackpad
//  double volSize = bb.range().length() / pow(3, 1.0/3.0); // mm size of volume
  double scale = 180/padSize;

  camera->Azimuth(-scale * delta.x());
  camera->Elevation(scale * delta.y());
//
//  ssc::Vector3D t = scale * (-delta.x() * e_x + delta.y() * e_y);
//
//  position += t;
//  focus += t;
//
//  camera->SetPosition(position.begin());
//  camera->SetFocalPoint(focus.begin());
}

void CameraControlWidget::dollySlot()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;
  double factor = action->data().toDouble();
  this->getCamera()->Dolly(factor);
}

void CameraControlWidget::panSlot(QPoint delta)
{
  vtkCameraPtr camera = this->getCamera();
  ssc::Vector3D position(camera->GetPosition());
  ssc::Vector3D focus(camera->GetFocalPoint());
  ssc::Vector3D vup(camera->GetViewUp());

  ssc::Vector3D e_x = cross(focus-position, vup).normal();
  ssc::Vector3D e_y = vup.normal();

  ssc::DoubleBoundingBox3D bb(viewManager()->get3DView()->getRenderer()->ComputeVisiblePropBounds());

  int padSize = (mPanWidget->size().width() + mPanWidget->size().height())/2.0; // pixel size of trackpad
  double volSize = bb.range().length() / pow(3, 1.0/3.0); // mm size of volume
  //int padSize = 100; // pixel size of trackpad
  //double volSize = 200; // mm size of volume
  double scale = volSize/padSize;
  ssc::Vector3D t = scale * (-delta.x() * e_x + delta.y() * e_y);

  position += t;
  focus += t;

  camera->SetPosition(position.begin());
  camera->SetFocalPoint(focus.begin());
}

CameraControlWidget::~CameraControlWidget()
{
}

//void ImagePropertiesWidget::updateSlot()
//{
//  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//  if (image)
//  {
//    mImageNameLabel->setText(qstring_cast(image->getName()));
//  }
//}

void CameraControlWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void CameraControlWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx
