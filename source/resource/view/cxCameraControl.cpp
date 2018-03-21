/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * cxCameraControl.cpp
 *
 *  \date Oct 15, 2010
 *      \author christiana
 */
#include "cxCameraControl.h"

#include <QAction>
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "cxView.h"
#include <QDomNode>
#include "cxTypeConversions.h"
#include "cxLogger.h"

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

	this->addTextElement(dataNode, "position", qstring_cast(Vector3D(mCamera->GetPosition())));
	this->addTextElement(dataNode, "focalPoint", qstring_cast(Vector3D(mCamera->GetFocalPoint())));
	this->addTextElement(dataNode, "viewUp", qstring_cast(Vector3D(mCamera->GetViewUp())));
	this->addTextElement(dataNode, "nearClip", qstring_cast(mCamera->GetClippingRange()[0]));
	this->addTextElement(dataNode, "farClip", qstring_cast(mCamera->GetClippingRange()[1]));
	this->addTextElement(dataNode, "parallelScale", qstring_cast(mCamera->GetParallelScale()));
}

void CameraData::parseXml(QDomNode dataNode)
{
	Vector3D position = Vector3D::fromString(dataNode.namedItem("position").toElement().text());
	Vector3D focalPoint = Vector3D::fromString(dataNode.namedItem("focalPoint").toElement().text());
	Vector3D viewUp = Vector3D::fromString(dataNode.namedItem("viewUp").toElement().text());
	double nearClip = dataNode.namedItem("nearClip").toElement().text().toDouble();
	double farClip = dataNode.namedItem("farClip").toElement().text().toDouble();
	double parallelScale = dataNode.namedItem("parallelScale").toElement().text().toDouble();

	if (similar(viewUp.length(), 0.0))
		return; // ignore reading if undefined data
	double LARGE_NUMBER = 1.0E6; // corresponding to a distance of 1km - unphysical for human-sized data
	if ((position-focalPoint).length() > LARGE_NUMBER)
		return;
	if (focalPoint.length() > LARGE_NUMBER)
		return;
	if (fabs(parallelScale) > LARGE_NUMBER)
		return;

	this->getCamera();

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


CameraControl::CameraControl(QObject* parent) :
    QObject(parent),
	mSuperiorViewAction(NULL),
	mAnteriorViewAction(NULL)
{
}

CameraControl::~CameraControl()
{
}

/*Move the camera focus to p_r. Keep the view direction and distance constant
 * (i.e. keep pos of camera constant relative to focus).
 *
 */
void CameraControl::translateByFocusTo(Vector3D p_r)
{
	vtkCameraPtr camera = this->getCamera();
	if (!camera)
		return;

	Vector3D f(camera->GetFocalPoint());
	Vector3D p(camera->GetPosition());
	Vector3D delta = p_r - f;
	f += delta;
	p += delta;
	camera->SetFocalPoint(f.begin());
	camera->SetPosition(p.begin());
}

void CameraControl::setSuperiorView() const
{
    if(mSuperiorViewAction)
		mSuperiorViewAction->trigger();
}

void CameraControl::setAnteriorView() const
{
	if(mAnteriorViewAction)
		mAnteriorViewAction->trigger();
}

QActionGroup* CameraControl::createStandard3DViewActions()
{
	QActionGroup* group = new QActionGroup(this);
	mAnteriorViewAction = this->addStandard3DViewAction("A", "Anterior View", Vector3D(0, 1, 0), group);
	this->addStandard3DViewAction("P", "Posterior View", Vector3D(0, -1, 0), group);
	mSuperiorViewAction = this->addStandard3DViewAction("S", "Superior View", Vector3D(0, 0, -1), group);
	this->addStandard3DViewAction("I", "Inferior View", Vector3D(0, 0, 1), group);
	this->addStandard3DViewAction("L", "Left View", Vector3D(-1, 0, 0), group);
	this->addStandard3DViewAction("R", "Right View", Vector3D(1, 0, 0), group);
	this->addStandard3DViewAction("O", "Orthogonal View", Vector3D(-1, 1, -1).normal(), group);
	return group;
}

/** Add one layout as an action to the layout menu.
 */
QAction* CameraControl::addStandard3DViewAction(QString caption, QString help, Vector3D viewDirection,
	QActionGroup* group)
{
	QAction* action = new QAction(help, group);
	action->setStatusTip(help);
	action->setWhatsThis(help);
	action->setIcon(QIcon(":/icons/camera_view_" + caption + ".png"));
	//  QFont font;
	//  font.setBold(true);
	//  if (font.pointSize()>=0)
	//    font.setPointSize(font.pointSize()*1.5);
	//  action->setFont(font);
	action->setData(QVariant(qstring_cast(viewDirection)));
	connect(action, &QAction::triggered, this, &CameraControl::setStandard3DViewActionSlot);
	return action;
}

void CameraControl::refreshView(ViewPtr view)
{
	this->setView(view);
	if(view)
		view->getRenderer()->ResetCameraClippingRange();
}

void CameraControl::setView(ViewPtr view)
{
	mView = view;
}

ViewPtr CameraControl::getView() const
{
	return mView;
}

vtkRendererPtr CameraControl::getRenderer() const
{
	if (!mView)
		return vtkRendererPtr();
	return mView->getRenderer();
}
vtkCameraPtr CameraControl::getCamera() const
{
	if (!this->getRenderer())
		return vtkCameraPtr();
	return this->getRenderer()->GetActiveCamera();
}

void CameraControl::setStandard3DViewActionSlot()
{
	QAction* action = dynamic_cast<QAction*> (sender());
	if (!action)
		return;
	Vector3D viewDirection = Vector3D::fromString(action->data().toString());

	vtkRendererPtr renderer = this->getRenderer();
	if (!renderer)
		return;
	vtkCameraPtr camera = this->getCamera();

	renderer->ResetCamera();

	Vector3D focus(camera->GetFocalPoint());
	Vector3D pos = focus - 500 * viewDirection;
	Vector3D vup(0, 0, 1);
	//Vector3D dir = (focus-direction).normal();

	Vector3D left = cross(vup, viewDirection);
//	CX_LOG_CHANNEL_DEBUG("CA") << "    cross(vup, viewDirection) " << cross(vup, viewDirection);
	if (similar(left.length(), 0.0))
		left = Vector3D(1, 0, 0);
	vup = cross(viewDirection, left).normal();

//	CX_LOG_CHANNEL_DEBUG("CA") << "CameraControl::setStandard3DViewActionSlot()";
//	CX_LOG_CHANNEL_DEBUG("CA") << "    viewDirection " << viewDirection;
//	CX_LOG_CHANNEL_DEBUG("CA") << "    left " << left;
//	CX_LOG_CHANNEL_DEBUG("CA") << "    vup " << vup;

	camera->SetPosition(pos.begin());
	camera->SetViewUp(vup.begin());

	renderer->ResetCamera(); // let vtk do the zooming base work
	camera->Dolly(1.5); // zoom in a bit more than the default vtk value
	renderer->ResetCameraClippingRange();
}

} // namespace cx
