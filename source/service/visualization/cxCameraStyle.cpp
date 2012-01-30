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

namespace cx
{

CameraStyle::CameraStyle() :
	mCameraStyle(DEFAULT_STYLE), mCameraOffset(-1)
{
	connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(viewChangedSlot()));

	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
	this->dominantToolChangedSlot();
}

View3D* CameraStyle::getView() const
{
	return viewManager()->get3DView();
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

void CameraStyle::setCameraStyle(Style style, int offset)
{
	if (mCameraStyle == style)
		return;

	switch (style)
	{
	case DEFAULT_STYLE:
		this->activateCameraDefaultStyle();
		break;
	case TOOL_STYLE:
		this->activateCameraToolStyle(offset);
		break;
	case ANGLED_TOOL_STYLE:
		this->activateCameraAngledToolStyle(offset);
		break;
	default:
		break;
	};
}

void CameraStyle::setCameraOffsetSlot(int offset)
{
	if (offset != -1)
		mCameraOffset = std::max<int>(offset, 1.0);
}

void CameraStyle::moveCameraToolStyleSlot(ssc::Transform3D prMt, double timestamp)
{
	ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();

	ssc::Transform3D rMt = rMpr * prMt;

	double offset = mFollowingTool->getTooltipOffset();

	ssc::Vector3D camera_r = rMt.coord(ssc::Vector3D(0, 0, offset - mCameraOffset));
	ssc::Vector3D focus_r = rMt.coord(ssc::Vector3D(0, 0, offset));
	ssc::Vector3D vup_r = rMt.vector(ssc::Vector3D(-1, 0, 0));
	if (mCameraStyle == ANGLED_TOOL_STYLE)
	{
		double height = mCameraOffset * tan(20 / 180.0 * M_PI);
		camera_r += vup_r * height;
	}

	vtkCameraPtr camera = this->getCamera();
	if (!camera)
		return;
	camera->SetPosition(camera_r.begin());
	camera->SetFocalPoint(focus_r.begin());
	camera->SetViewUp(vup_r.begin());

	camera->SetClippingRange(1, std::max<double>(1000, mCameraOffset * 1.5));
}

/**reset the view connection, this is in case the view, reps or tool has been deleted/recreated in
 * the layout change process.
 */
void CameraStyle::viewChangedSlot()
{
	this->disconnectTool();
	this->connectTool();
}

void CameraStyle::activateCameraDefaultStyle()
{
	this->disconnectTool();

	if (!this->getRenderer())
		return;

	mCameraStyle = DEFAULT_STYLE;

	ssc::messageManager()->sendInfo("Default camera style activated.");
}

void CameraStyle::activateCameraToolStyle(int offset)
{
	this->disconnectTool();
	this->setCameraOffsetSlot(offset);
	mCameraStyle = TOOL_STYLE;
	this->connectTool();
	ssc::messageManager()->sendInfo("Tool camera style activated.");
}

void CameraStyle::activateCameraAngledToolStyle(int offset)
{
	this->disconnectTool();
	this->setCameraOffsetSlot(offset);
	mCameraStyle = ANGLED_TOOL_STYLE;
	this->connectTool();
	ssc::messageManager()->sendInfo("Angled tool camera style activated.");
}

void CameraStyle::dominantToolChangedSlot()
{
	ssc::ToolPtr newTool = ssc::toolManager()->getDominantTool();
	if (newTool == mFollowingTool)
		return;

	this->disconnectTool();
	this->connectTool();
}

void CameraStyle::connectTool()
{
	if (mCameraStyle == DEFAULT_STYLE)
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
	if (mCameraStyle == TOOL_STYLE)
		rep->setStayHiddenAfterVisible(true);

	ssc::messageManager()->sendInfo("Camera is following " + mFollowingTool->getName());
}

void CameraStyle::disconnectTool()
{
	if (mCameraStyle == DEFAULT_STYLE)
		return;

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

}//namespace cx
