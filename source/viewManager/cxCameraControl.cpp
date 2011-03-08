/*
 * cxCameraControl.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: christiana
 */
#include "cxCameraControl.h"


#include <QAction>
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"
#include "cxViewManager.h"
#include "cxView3D.h"
//#include "cxDataInterface.h"

namespace cx
{



CameraData::CameraData()
{
}

void CameraData::setCamera(vtkCameraPtr camera)
{
  mCamera = camera;
}

vtkCameraPtr CameraData::getCamera() const
{
  if (!mCamera)
    mCamera = vtkCameraPtr::New();
  return mCamera;
}

void CameraData::addTextElement(QDomNode parentNode, QString name, QString value) const
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement node = doc.createElement(name);
  node.appendChild(doc.createTextNode(value));
  parentNode.appendChild(node);
}

void CameraData::addXml(QDomNode dataNode) const
{
  if (!mCamera)
    return;

  this->addTextElement(dataNode, "position",      qstring_cast(ssc::Vector3D(mCamera->GetPosition())));
  this->addTextElement(dataNode, "focalPoint",    qstring_cast(ssc::Vector3D(mCamera->GetFocalPoint())));
  this->addTextElement(dataNode, "viewUp",        qstring_cast(ssc::Vector3D(mCamera->GetViewUp())));
  this->addTextElement(dataNode, "nearClip",      qstring_cast(mCamera->GetClippingRange()[0]));
  this->addTextElement(dataNode, "farClip",       qstring_cast(mCamera->GetClippingRange()[1]));
  this->addTextElement(dataNode, "parallelScale", qstring_cast(mCamera->GetParallelScale()));
}

void CameraData::parseXml(QDomNode dataNode)
{
  ssc::Vector3D vup = ssc::Vector3D::fromString(dataNode.namedItem("viewUp").toElement().text());
  if (ssc::similar(vup.length(), 0.0))
    return; // ignore reading if undefined data

  this->getCamera();

  ssc::Vector3D position =   ssc::Vector3D::fromString(dataNode.namedItem("position").toElement().text());
  ssc::Vector3D focalPoint = ssc::Vector3D::fromString(dataNode.namedItem("focalPoint").toElement().text());
  ssc::Vector3D viewUp =     ssc::Vector3D::fromString(dataNode.namedItem("viewUp").toElement().text());
  double nearClip =      dataNode.namedItem("nearClip").toElement().text().toDouble();
  double farClip =       dataNode.namedItem("farClip").toElement().text().toDouble();
  double parallelScale = dataNode.namedItem("parallelScale").toElement().text().toDouble();

  mCamera->SetClippingRange(nearClip, farClip);
  mCamera->SetPosition(position.begin());
  mCamera->SetFocalPoint(focalPoint.begin());
  mCamera->ComputeViewPlaneNormal();
  mCamera->SetViewUp(viewUp.begin());
  mCamera->SetParallelScale(parallelScale);
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------



CameraControl::CameraControl(QObject* parent) : QObject(parent)
{

}

CameraControl::~CameraControl()
{

}

/*Move the camera focus to p_r. Keep the view direction and distance constant
 * (i.e. keep pos of camera constant relative to focus).
 *
 */
void CameraControl::translateByFocusTo(ssc::Vector3D p_r)
{
  vtkCameraPtr camera = this->getCamera();
  if (!camera)
    return;

  ssc::Vector3D f(camera->GetFocalPoint());
  ssc::Vector3D p(camera->GetPosition());
  ssc::Vector3D delta = p_r-f;
  f += delta;
  p += delta;
  camera->SetFocalPoint(f.begin());
  camera->SetPosition(p.begin());
}

QActionGroup* CameraControl::createStandard3DViewActions()
{
  QActionGroup* group = new QActionGroup(this);
  this->addStandard3DViewAction("A", "Anterior View",   ssc::Vector3D( 0, 1, 0), group);
  this->addStandard3DViewAction("P", "Posterior View",  ssc::Vector3D( 0,-1, 0), group);
  this->addStandard3DViewAction("S", "Superior View",   ssc::Vector3D( 0, 0,-1), group);
  this->addStandard3DViewAction("I", "Inferior View",   ssc::Vector3D( 0, 0, 1), group);
  this->addStandard3DViewAction("L", "Left View",       ssc::Vector3D(-1, 0, 0), group);
  this->addStandard3DViewAction("R", "Right View",      ssc::Vector3D( 1, 0, 0), group);
  this->addStandard3DViewAction("O", "Orthogonal View", ssc::Vector3D(-1, 1,-1).normal(), group);
  return group;
}

/** Add one layout as an action to the layout menu.
 */
QAction* CameraControl::addStandard3DViewAction(QString caption, QString help, ssc::Vector3D viewDirection, QActionGroup* group)
{
  QAction* action = new QAction(help, group);
  action->setStatusTip(help);
  action->setWhatsThis(help);
  action->setIcon(QIcon(":/icons/camera_view_"+caption+".png"));
//  QFont font;
//  font.setBold(true);
//  if (font.pointSize()>=0)
//    font.setPointSize(font.pointSize()*1.5);
//  action->setFont(font);
  action->setData(QVariant(qstring_cast(viewDirection)));
  connect(action, SIGNAL(triggered()), this, SLOT(setStandard3DViewActionSlot()));
  return action;
}

vtkRendererPtr CameraControl::getRenderer() const
{
  if (!viewManager()->get3DView())
    return vtkRendererPtr();
  return viewManager()->get3DView()->getRenderer();
}
vtkCameraPtr CameraControl::getCamera() const
{
  if (!this->getRenderer())
    return vtkCameraPtr();
  return this->getRenderer()->GetActiveCamera();
}

void CameraControl::setStandard3DViewActionSlot()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;
  ssc::Vector3D viewDirection = ssc::Vector3D::fromString(action->data().toString());

  vtkRendererPtr renderer = this->getRenderer();
  if (!renderer)
    return;
  vtkCameraPtr camera = this->getCamera();

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

} // namespace cx
