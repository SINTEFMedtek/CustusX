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

#include "cxCameraStyleForView.h"

#include <vtkRenderer.h>
#include <vtkCamera.h>

#include "cxTrackingService.h"
#include "cxToolRep3D.h"
#include "cxView.h"
#include "boost/bind.hpp"
#include <vtkRenderWindow.h>
#include "vtkInteractorStyleUnicam.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "cxCoreServices.h"
#include "cxViewportListener.h"

#include "cxTool.h"
#include <vtkRenderWindowInteractor.h>
#include "cxPatientModelService.h"
#include "cxRepContainer.h"
#include "cxLogger.h"

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

CameraStyleForView::CameraStyleForView(CoreServicesPtr backend) :
	mCameraStyleForView(cstDEFAULT_STYLE),
	mBlockCameraUpdate(false),
	mBackend(backend)
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&CameraStyleForView::viewportChangedSlot, this));

	mPreRenderListener.reset(new ViewportPreRenderListener);
	mPreRenderListener->setCallback(boost::bind(&CameraStyleForView::onPreRender, this));

	connect(mBackend->tracking().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(activeToolChangedSlot()));
}

void CameraStyleForView::setView(ViewPtr widget)
{
	this->disconnectTool();
	mView = widget;
	this->connectTool();
}


ViewPtr CameraStyleForView::getView() const
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

	ToolRep3DPtr rep = RepContainer(this->getView()->getReps()).findFirst<ToolRep3D>(mFollowingTool);
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

	Transform3D rMpr = mBackend->patient()->get_rMpr();

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
	Vector3D vup_old(camera->GetViewUp());

    if (similar(pos_old, camera_r, 0.1) && similar(focus_old, focus_r, 0.1) && similar(vup_old, vup_r,0.1 ))
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

void CameraStyleForView::activeToolChangedSlot()
{
	ToolPtr newTool = mBackend->tracking()->getActiveTool();
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

	mFollowingTool = mBackend->tracking()->getActiveTool();

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

	report("Camera is following " + mFollowingTool->getName());
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

	ViewPtr view = this->getView();
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

CAMERA_STYLE_TYPE CameraStyleForView::getCameraStyle()
{
	return mCameraStyleForView;
}

}//namespace cx
