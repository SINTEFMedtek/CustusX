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

#include "cxCameraStyleForView.h"

#include <vtkRenderer.h>
#include <vtkCamera.h>
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "sscToolManager.h"
#include "sscToolRep3D.h"
#include "sscDataManager.h"
#include "sscView.h"
#include "boost/bind.hpp"
#include <vtkRenderWindow.h>
#include "vtkInteractorStyleUnicam.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "cxVisualizationServiceBackend.h"
#include "sscViewportListener.h"
#include "sscLogger.h"

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

CameraStyleForView::CameraStyleForView(VisualizationServiceBackendPtr backend) :
	mCameraStyleForView(cstDEFAULT_STYLE),
	mBlockCameraUpdate(false),
	mBackend(backend)
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&CameraStyleForView::viewportChangedSlot, this));

	mPreRenderListener.reset(new ViewportPreRenderListener);
	mPreRenderListener->setCallback(boost::bind(&CameraStyleForView::onPreRender, this));

	connect(mBackend->getToolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
}

void CameraStyleForView::setView(ViewWidgetQPtr widget)
{
	this->disconnectTool();
	mView = widget;
	this->connectTool();
}


ViewWidgetQPtr CameraStyleForView::getView() const
{
	return mView;
}

void CameraStyleForView::viewportChangedSlot()
{
	if (mBlockCameraUpdate)
		return;
	this->updateCamera();
}

void CameraStyleForView::onPreRender()
{
	if (mFollowingTool)
		this->moveCameraToolStyleSlot(mFollowingTool->get_prMt(), mFollowingTool->getTimestamp());
}

ToolRep3DPtr CameraStyleForView::getToolRep() const
{
	if (!this->getView())
		return ToolRep3DPtr();

	ToolRep3DPtr rep = RepManager::findFirstRep<ToolRep3D>(this->getView()->getReps(), mFollowingTool);
	return rep;
}

vtkRendererPtr CameraStyleForView::getRenderer() const
{
	if (!this->getView())
		return vtkRendererPtr();
	return this->getView()->getRenderer();
}

vtkCameraPtr CameraStyleForView::getCamera() const
{
	if (!this->getRenderer())
		return vtkCameraPtr();
	return this->getRenderer()->GetActiveCamera();
}

void CameraStyleForView::setModified()
{
	mPreRenderListener->setModified();
}

void CameraStyleForView::updateCamera()
{
	this->setModified();
}

void CameraStyleForView::moveCameraToolStyleSlot(Transform3D prMt, double timestamp)
{
	if (mCameraStyleForView == cstDEFAULT_STYLE)
		return;
	if (!mFollowingTool)
		return;


	vtkCameraPtr camera = this->getCamera();
	if (!camera)
		return;

	Transform3D rMpr = mBackend->getDataManager()->get_rMpr();

	Transform3D rMt = rMpr * prMt;

	double offset = mFollowingTool->getTooltipOffset();

	double cameraOffset = camera->GetDistance();

//	std::cout << "cameraOffset pre " << cameraOffset << std::endl;
//	std::cout << "rMt\n" << rMt << std::endl;
	Vector3D camera_r = rMt.coord(Vector3D(0, 0, offset - cameraOffset));
	Vector3D focus_r = rMt.coord(Vector3D(0, 0, offset));
//	std::cout << "cameraOffset ppost " << (focus_r-camera_r).length() << std::endl;
	Vector3D vup_r = rMt.vector(Vector3D(-1, 0, 0));
	if (mCameraStyleForView == cstANGLED_TOOL_STYLE)
	{
		// elevate 20*, but keep distance
		double height = cameraOffset * tan(20 / 180.0 * M_PI);
		camera_r += vup_r * height;
		Vector3D elevated = camera_r + vup_r * height;
		Vector3D n_foc2eye = (elevated - focus_r).normalized();
		camera_r = focus_r + cameraOffset * n_foc2eye;
	}

	Vector3D pos_old(camera->GetPosition());
	Vector3D focus_old(camera->GetFocalPoint());

	if (similar(pos_old, camera_r, 1) && similar(focus_old, focus_r, 1))
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

void CameraStyleForView::dominantToolChangedSlot()
{
	ToolPtr newTool = mBackend->getToolManager()->getDominantTool();
	if (newTool == mFollowingTool)
		return;

	this->disconnectTool();
	this->connectTool();
}

bool CameraStyleForView::isToolFollowingStyle(CAMERA_STYLE_TYPE style) const
{
	return ( style==cstTOOL_STYLE )||( style==cstANGLED_TOOL_STYLE);
}

void CameraStyleForView::connectTool()
{
	if (!this->isToolFollowingStyle(mCameraStyleForView))
		return;

	mFollowingTool = mBackend->getToolManager()->getDominantTool();

	if (!mFollowingTool)
		return;

	if (!this->getView())
		return;

	connect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(setModified()));

	ToolRep3DPtr rep = this->getToolRep();
	if (rep)
	{
		rep->setOffsetPointVisibleAtZeroOffset(true);
		if (mCameraStyleForView == cstTOOL_STYLE)
			rep->setStayHiddenAfterVisible(true);
	}

	mViewportListener->startListen(this->getRenderer());
	mPreRenderListener->startListen(this->getRenderer());

	this->updateCamera();

	messageManager()->sendInfo("Camera is following " + mFollowingTool->getName());
}

void CameraStyleForView::disconnectTool()
{
	if (mCameraStyleForView == cstDEFAULT_STYLE)
		return;

	mViewportListener->stopListen();
	mPreRenderListener->stopListen();

	if (mFollowingTool)
	{
		disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(setModified()));

		ToolRep3DPtr rep = this->getToolRep();
		if (rep)
			rep->setStayHiddenAfterVisible(false);
	}

	mFollowingTool.reset();
}

void CameraStyleForView::setCameraStyle(CAMERA_STYLE_TYPE style)
{
	if (mCameraStyleForView == style)
		return;

	this->disconnectTool();

	ViewWidget* view = this->getView();
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

	mCameraStyleForView = style;

	this->connectTool();
}

}//namespace cx
