/*
 * cxCameraControlWidget.cpp
 *
 *  Created on: Oct 7, 2010
 *      Author: christiana
 */

#include "cxCameraControlWidget.h"

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

namespace cx
{

MousePadWidget::MousePadWidget(QWidget* parent, QSize minimumSize) : QFrame(parent), mFixPosX(false), mMinSize(minimumSize)
{
}

MousePadWidget::~MousePadWidget()
{
}

void MousePadWidget::setFixedXPos(bool on)
{
  mFixPosX = on;
  this->fixPos();
}

void MousePadWidget::fixPos()
{
  if (mFixPosX)
  {
    mLastPos.rx() = this->width()/2;
  }
}

void MousePadWidget::showEvent(QShowEvent* event)
{
  mLastPos = QPoint(this->width()/2, this->height()/2);
  this->fixPos();
  this->update();
}

void MousePadWidget::mousePressEvent(QMouseEvent* event)
{
  mLastPos = event->pos();
  this->fixPos();
  this->update();
}
void MousePadWidget::mouseMoveEvent(QMouseEvent* event)
{
  QPoint delta = event->pos() - mLastPos;

  double padSize = (this->size().width() + this->size().height())/2.0; // pixel size of trackpad
  QPointF deltaN(double(delta.x())/padSize, double(delta.y())/padSize);
  emit mouseMoved(deltaN);

  mLastPos = event->pos();
  this->fixPos();
  this->update();
}
void MousePadWidget::mouseReleaseEvent(QMouseEvent* event)
{
  mLastPos = QPoint(this->width()/2, this->height()/2);
  this->fixPos();
  this->update();
}

void MousePadWidget::resizeEvent(QResizeEvent* event)
{
  mLastPos = QPoint(this->width()/2, this->height()/2);
  this->fixPos();
  this->update();
}

void MousePadWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    ssc::Vector3D center(this->width()/2, this->height()/2, 0);
    ssc::Vector3D delta = ssc::Vector3D(mLastPos.x(), mLastPos.y(), 0) - center;
    double radius = center.length();// - delta.length();
    QPoint qcenter(this->width()/2, this->height()/2);


    //QRadialGradient radialGrad(mLastPos, radius, qcenter);
    QRadialGradient radialGrad(qcenter, radius, mLastPos);
    //radialGrad.setColorAt(0, QColor("ivory"));
    radialGrad.setColorAt(0.0, QColor("khaki"));
    radialGrad.setColorAt(0.4, QColor("lightgrey"));
    radialGrad.setColorAt(1, QColor("dimgrey"));
//    radialGrad.setColorAt(0, Qt::red);
//    radialGrad.setColorAt(0.5, Qt::blue);
//    radialGrad.setColorAt(1, Qt::green);

//    p.drawImage(0, 0, m_shade);
    QColor color(146, 0, 146);
    QBrush brush(radialGrad);

    p.setPen(QColor(146, 0, 146));
    p.setBrush(QColor(146, 0, 146));
    p.fillRect(0, 0, width() - 1, height() - 1, brush);
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


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


CameraControlWidget::CameraControlWidget(QWidget* parent) :
    QWidget(parent)
{
//  this->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
  this->setObjectName("CameraControlWidget");
  this->setWindowTitle("Camera Control");

  mMinPadSize = QSize(50,50);
  mMinBarSize = QSize(20,50);

  //layout
  mTopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  this->createStandard3DViewActions();
  this->definePanLayout();
  this->defineRotateLayout();


//  mTopLayout->addStretch();

  //connect(ssc::dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(updateSlot()));
  //updateSlot();
}

void CameraControlWidget::createStandard3DViewActions()
{
  QActionGroup* group = new QActionGroup(this);
  this->addStandard3DViewAction("A", "Anterior View",   ssc::Vector3D( 0, 1, 0), group);
  this->addStandard3DViewAction("P", "Posterior View",  ssc::Vector3D( 0,-1, 0), group);
  this->addStandard3DViewAction("S", "Superior View",   ssc::Vector3D( 0, 0,-1), group);
  this->addStandard3DViewAction("I", "Inferior View",   ssc::Vector3D( 0, 0, 1), group);
  this->addStandard3DViewAction("L", "Left View",       ssc::Vector3D(-1, 0, 0), group);
  this->addStandard3DViewAction("R", "Right View",      ssc::Vector3D( 1, 0, 0), group);
  this->addStandard3DViewAction("O", "Orthogonal View", ssc::Vector3D(-1, 1,-1).normal(), group);

  QToolBar* toolBar = new QToolBar(this);
  mTopLayout->addWidget(toolBar);
  toolBar->addActions(group->actions());
  toolBar->addSeparator();
}

/** Add one layout as an action to the layout menu.
 */
QAction* CameraControlWidget::addStandard3DViewAction(QString caption, QString help, ssc::Vector3D viewDirection, QActionGroup* group)
{
  QAction* action = new QAction(caption, group);
  action->setStatusTip(help);
  action->setWhatsThis(help);
  QFont font;
  font.setBold(true);
  if (font.pointSize()>=0)
    font.setPointSize(font.pointSize()*1.5);
  action->setFont(font);
  action->setData(QVariant(qstring_cast(viewDirection)));
  connect(action, SIGNAL(triggered()), this, SLOT(setStandard3DViewActionSlot()));
  return action;
}

void CameraControlWidget::setStandard3DViewActionSlot()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;
  ssc::Vector3D viewDirection = ssc::Vector3D::fromString(action->data().toString());

  vtkRendererPtr renderer = viewManager()->get3DView()->getRenderer();
  vtkCameraPtr camera = renderer->GetActiveCamera();

  renderer->ResetCamera();

  ssc::Vector3D focus(camera->GetFocalPoint());
  ssc::Vector3D pos = focus - 500 * viewDirection;
  ssc::Vector3D vup(0,0,1);
  //ssc::Vector3D dir = (focus-direction).normal();

  ssc::Vector3D left = cross(vup, viewDirection);
  if (ssc::similar(left.length(), 0.0))
    left = ssc::Vector3D(1,0,0);
  vup = cross(viewDirection, left).normal();

  camera->SetPosition(pos.begin());
  camera->SetViewUp(vup.begin());

  renderer->ResetCamera(); // let vtk do the zooming base work
  camera->Dolly(1.5); // zoom in a bit more than the default vtk value
  renderer->ResetCameraClippingRange();
}

void CameraControlWidget::defineRotateLayout()
{
  QGroupBox* group = new QGroupBox("rotate", this);
  group->setFlat(true);
  mTopLayout->addWidget(group);

  QHBoxLayout* layout = new QHBoxLayout;
  layout->setMargin(4);
  group->setLayout(layout);

  MousePadWidget* rotateWidget = new MousePadWidget(this, mMinPadSize);
  rotateWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  rotateWidget->setLineWidth(3);
  connect(rotateWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(rotateXZSlot(QPointF)));
  layout->addWidget(rotateWidget, 4);

  MousePadWidget* rotateYWidget = new MousePadWidget(this, mMinBarSize);
  rotateYWidget->setFixedXPos(true);
  rotateYWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  rotateYWidget->setLineWidth(3);
  connect(rotateYWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(rotateYSlot(QPointF)));
  layout->addWidget(rotateYWidget, 1);
}


void CameraControlWidget::definePanLayout()
{
  QGroupBox* group = new QGroupBox("pan", this);
  group->setFlat(true);
  mTopLayout->addWidget(group);

  QHBoxLayout* panLayout = new QHBoxLayout;
  panLayout->setMargin(4);
  group->setLayout(panLayout);

  MousePadWidget* panWidget = new MousePadWidget(this, mMinPadSize);
  panWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  panWidget->setLineWidth(3);
  connect(panWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(panXZSlot(QPointF)));
  panLayout->addWidget(panWidget, 4);

  MousePadWidget* dollyWidget = new MousePadWidget(this, mMinBarSize);
  dollyWidget->setFixedXPos(true);
  dollyWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  dollyWidget->setLineWidth(3);
  connect(dollyWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(dollySlot(QPointF)));
  panLayout->addWidget(dollyWidget, 1);
}

vtkCameraPtr CameraControlWidget::getCamera() const
{
  return viewManager()->get3DView()->getRenderer()->GetActiveCamera();
}

void CameraControlWidget::rotateYSlot(QPointF delta)
{
  double scale = 180;
  double factor = scale * delta.y();

  this->getCamera()->Roll(factor);
}

void CameraControlWidget::rotateXZSlot(QPointF delta)
{
  vtkCameraPtr camera = this->getCamera();
  double scale = 180;

  camera->Azimuth(-scale * delta.x());
  camera->Elevation(scale * delta.y());
}

void CameraControlWidget::dollySlot(QPointF delta)
{
  double factor = 1 + delta.y();
  this->getCamera()->Dolly(factor);
  viewManager()->get3DView()->getRenderer()->ResetCameraClippingRange();
}

void CameraControlWidget::panXZSlot(QPointF delta)
{
  vtkCameraPtr camera = this->getCamera();
  ssc::Vector3D position(camera->GetPosition());
  ssc::Vector3D focus(camera->GetFocalPoint());
  ssc::Vector3D vup(camera->GetViewUp());

  ssc::Vector3D e_x = cross(focus-position, vup).normal();
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

CameraControlWidget::~CameraControlWidget()
{
}

void CameraControlWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void CameraControlWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx
