// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxCameraStyle.h"

#include "cxView3D.h"
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include "sscImage.h"
#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "sscToolManager.h"
#include "sscToolRep3D.h"
#include "sscTypeConversions.h"
#include "cxViewManager.h"
#include "sscView.h"
#include "sscTool.h"
#include "boost/bind.hpp"
#include <vtkRenderWindow.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleUnicam.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleFlight.h"

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, CAMERA_STYLE_TYPE, cstCOUNT)
{
	"DEFAULT_STYLE",
	"TOOL_STYLE",
	"ANGLED_TOOL_STYLE",
	"UNICAM_STYLE"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, CAMERA_STYLE_TYPE, cstCOUNT)


namespace cx
{

CameraStyle::CameraStyle() :
	mCameraStyle(cstDEFAULT_STYLE),
    mBlockCameraUpdate(false),
    mCameraStyleGroup(NULL)
{
	connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(viewChangedSlot()));

	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&CameraStyle::viewportChangedSlot, this));

	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
	this->dominantToolChangedSlot();
	this->viewChangedSlot();
}

View3D* CameraStyle::getView() const
{
	return viewManager()->get3DView();
}

void CameraStyle::viewportChangedSlot()
{
	if (mBlockCameraUpdate)
		return;

	this->updateCamera();

//	// debug stuff:
//	std::cout << "CameraStyle::viewportChangedSlot()" << std::endl;
//	vtkCameraPtr camera = this->getCamera();

//	ssc::Vector3D pos(camera->GetPosition());
//	ssc::Vector3D focus(camera->GetFocalPoint());
//	std::cout << "distance " << camera->GetDistance() << std::endl;
}

ssc::ToolRep3DPtr CameraStyle::getToolRep() const
{
	if (!this->getView())
		return ssc::ToolRep3DPtr();

	ssc::ToolRep3DPtr rep = RepManager::findFirstRep<ssc::ToolRep3D>(this->getView()->getReps(), mFollowingTool);
	return rep;
}

vtkRendererPtr CameraStyle::getRenderer() const
{
	if (!this->getView())
		return vtkRendererPtr();
	return this->getView()->getRenderer();
}

vtkCameraPtr CameraStyle::getCamera() const
{
	if (!this->getRenderer())
		return vtkCameraPtr();
	return this->getRenderer()->GetActiveCamera();
}

void CameraStyle::updateCamera()
{
	if (mFollowingTool)
		this->moveCameraToolStyleSlot(mFollowingTool->get_prMt(), mFollowingTool->getTimestamp());
}

void CameraStyle::moveCameraToolStyleSlot(ssc::Transform3D prMt, double timestamp)
{
	if (mCameraStyle == cstDEFAULT_STYLE)
		return;

	vtkCameraPtr camera = this->getCamera();
	if (!camera)
		return;

	ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();

	ssc::Transform3D rMt = rMpr * prMt;

	double offset = mFollowingTool->getTooltipOffset();

	double cameraOffset = camera->GetDistance();

//	std::cout << "cameraOffset pre " << cameraOffset << std::endl;
//	std::cout << "rMt\n" << rMt << std::endl;
	ssc::Vector3D camera_r = rMt.coord(ssc::Vector3D(0, 0, offset - cameraOffset));
	ssc::Vector3D focus_r = rMt.coord(ssc::Vector3D(0, 0, offset));
//	std::cout << "cameraOffset ppost " << (focus_r-camera_r).length() << std::endl;
	ssc::Vector3D vup_r = rMt.vector(ssc::Vector3D(-1, 0, 0));
	if (mCameraStyle == cstANGLED_TOOL_STYLE)
	{
		// elevate 20*, but keep distance
		double height = cameraOffset * tan(20 / 180.0 * M_PI);
		camera_r += vup_r * height;
		ssc::Vector3D elevated = camera_r + vup_r * height;
		ssc::Vector3D n_foc2eye = (elevated - focus_r).normalized();
		camera_r = focus_r + cameraOffset * n_foc2eye;
	}

	ssc::Vector3D pos_old(camera->GetPosition());
	ssc::Vector3D focus_old(camera->GetFocalPoint());

	if (ssc::similar(pos_old, camera_r, 1) && ssc::similar(focus_old, focus_r, 1))
		return; // break update loop: this event is triggered by camera change.

//	std::cout << "pos " << pos_old << " to " << camera_r << std::endl;
//	std::cout << "foc " << focus_old << " to " << focus_r << std::endl;

	mBlockCameraUpdate = true;
	camera->SetPosition(camera_r.begin());
	camera->SetFocalPoint(focus_r.begin());
	camera->SetViewUp(vup_r.begin());
	camera->SetClippingRange(1, std::max<double>(1000, cameraOffset * 1.5));
	mBlockCameraUpdate = false;
}

/**reset the view connection, this is in case the view, reps or tool has been deleted/recreated in
 * the layout change process.
 */
void CameraStyle::viewChangedSlot()
{
//	std::cout << "CameraStyle::viewChangedSlot()" << std::endl;
	this->disconnectTool();
	this->connectTool();
}

void CameraStyle::dominantToolChangedSlot()
{
	ssc::ToolPtr newTool = ssc::toolManager()->getDominantTool();
	if (newTool == mFollowingTool)
		return;

	this->disconnectTool();
	this->connectTool();
}

bool CameraStyle::isToolFollowingStyle(CAMERA_STYLE_TYPE style) const
{
	return ( style==cstTOOL_STYLE )||( style==cstANGLED_TOOL_STYLE);
}

void CameraStyle::connectTool()
{
	if (!this->isToolFollowingStyle(mCameraStyle))
		return;

	mFollowingTool = ssc::toolManager()->getDominantTool();

	if (!mFollowingTool)
		return;

	if (!this->getView())
		return;

	//Need the toolrep to get the direction the camera should point in
	ssc::ToolRep3DPtr rep = this->getToolRep();

	if (!rep)
		return; //cannot set the camera to follow a tool if that tool does not have a rep

	connect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
		SLOT(moveCameraToolStyleSlot(Transform3D, double)));

	rep->setOffsetPointVisibleAtZeroOffset(true);
	if (mCameraStyle == cstTOOL_STYLE)
		rep->setStayHiddenAfterVisible(true);

	mViewportListener->startListen(this->getRenderer());

	this->updateCamera();

	ssc::messageManager()->sendInfo("Camera is following " + mFollowingTool->getName());
}

void CameraStyle::disconnectTool()
{
	if (mCameraStyle == cstDEFAULT_STYLE)
		return;

	mViewportListener->stopListen();

	if (mFollowingTool)
	{
		disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(moveCameraToolStyleSlot(Transform3D, double)));

		ssc::ToolRep3DPtr rep = this->getToolRep();
		if (rep)
			rep->setStayHiddenAfterVisible(false);
	}

	mFollowingTool.reset();
}

QActionGroup* CameraStyle::createInteractorStyleActionGroup()
{
	if (mCameraStyleGroup)
		return mCameraStyleGroup;

	mCameraStyleGroup = new QActionGroup(this);
	mCameraStyleGroup->setExclusive(true);

	this->addInteractorStyleAction("Normal Camera", mCameraStyleGroup,
	                               enum2string(cstDEFAULT_STYLE),
	                               QIcon(":/icons/camera-n.png"),
	                               "Set 3D interaction to the normal camera-oriented style.");
	this->addInteractorStyleAction("Tool", mCameraStyleGroup,
	                               enum2string(cstTOOL_STYLE),
	                               QIcon(":/icons/camera-t.png"),
	                               "Camera following tool.");
	this->addInteractorStyleAction("Angled Tool", mCameraStyleGroup,
	                               enum2string(cstANGLED_TOOL_STYLE),
	                               QIcon(":/icons/camera-at.png"),
	                               "Camera following tool (Placed at an angle of 20 degrees).");
	this->addInteractorStyleAction("Unicam", mCameraStyleGroup,
	                               enum2string(cstUNICAM_STYLE),
	                               QIcon(":/icons/camera-u.png"),
	                               "Set 3D interaction to a single-button style, useful for touch screens.");

//	this->addInteractorStyleAction("Object", camGroup, "vtkInteractorStyleTrackballActor",
//					QIcon(":/icons/camera-o.png"), "Set 3D interaction to a object-oriented style.");
//	this->addInteractorStyleAction("Flight", camGroup, "vtkInteractorStyleFlight", QIcon(":/icons/camera-f.png"),
//					"Set 3D interaction to a flight style.");

	return mCameraStyleGroup;
}

void CameraStyle::addInteractorStyleAction(QString caption, QActionGroup* group, QString uid, QIcon icon,
				QString helptext)
{
	vtkRenderWindowInteractor* interactor = NULL;
	ssc::ViewWidget* view = this->getView();
	if (view)
		interactor = view->getRenderWindow()->GetInteractor();

	QAction* action = new QAction(caption, group);
	action->setIcon(icon);
	action->setCheckable(true);
	action->setData(uid);
	action->setToolTip(helptext);
	action->setWhatsThis(helptext);
	if (interactor)
		action->setChecked(enum2string(mCameraStyle) == uid);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(setInteractionStyleActionSlot()));
}

void CameraStyle::updateActionGroup()
{
	QList<QAction*> actions = mCameraStyleGroup->actions();
	for (int i=0; i<actions.size(); ++i)
	{
		actions[i]->blockSignals(true);
		actions[i]->setChecked(actions[i]->data().toString() == enum2string(mCameraStyle));
		actions[i]->blockSignals(false);
	}
}

void CameraStyle::setInteractionStyleActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());
	if(!theAction)
		return;

	QString uid = theAction->data().toString();
	CAMERA_STYLE_TYPE newStyle = string2enum<cx::CAMERA_STYLE_TYPE>(uid);
	if (newStyle==cstCOUNT)
		return;

	this->setCameraStyle(newStyle);
}

void CameraStyle::setCameraStyle(CAMERA_STYLE_TYPE style)
{
	if (mCameraStyle == style)
		return;

	this->disconnectTool();

	ssc::ViewWidget* view = this->getView();
	if (!view)
		return;
	vtkRenderWindowInteractor* interactor = view->getRenderWindow()->GetInteractor();

	switch (style)
	{
	case cstDEFAULT_STYLE:
	case cstTOOL_STYLE:
	case cstANGLED_TOOL_STYLE:
		interactor->SetInteractorStyle(vtkInteractorStyleTrackballCameraPtr::New());
		break;
	case cstUNICAM_STYLE:
		interactor->SetInteractorStyle(vtkInteractorStyleUnicamPtr::New());
	default:
		break;
	};

	mCameraStyle = style;

	this->connectTool();
	this->updateActionGroup();
	ssc::messageManager()->sendInfo(QString("Activated camera style %1.").arg(enum2string(style)));
}

}//namespace cx
