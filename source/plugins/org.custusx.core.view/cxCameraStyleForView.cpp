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
#include <vtkLightCollection.h>
#include <vtkLight.h>

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
#include "cxRegionOfInterestMetric.h"
#include "cxNavigationAlgorithms.h"
#include "cxDoubleRange.h"

namespace cx
{

CameraStyleForView::CameraStyleForView(CoreServicesPtr backend) :
	mBlockCameraUpdate(false),
	mBackend(backend)
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&CameraStyleForView::viewportChangedSlot, this));

	mPreRenderListener.reset(new ViewportPreRenderListener);
	mPreRenderListener->setCallback(boost::bind(&CameraStyleForView::onPreRender, this));

	connect(mBackend->tracking().get(), &TrackingService::activeToolChanged,
			this, &CameraStyleForView::activeToolChangedSlot);
}

void CameraStyleForView::setView(ViewPtr widget)
{
	mViewportListener->stopListen();
	mPreRenderListener->stopListen();

	this->disconnectTool();
	mView = widget;
	this->connectTool();

	mViewportListener->startListen(this->getRenderer());
	mPreRenderListener->startListen(this->getRenderer());
}


ViewPtr CameraStyleForView::getView() const
{
	return mView;
}

void CameraStyleForView::viewportChangedSlot()
{
	if (mBlockCameraUpdate)
		return;
	this->setModified();
}

void CameraStyleForView::onPreRender()
{
	this->applyCameraStyle();
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

void CameraStyleForView::applyCameraStyle()
{
	vtkCameraPtr camera = this->getCamera();
	if (!camera)
		return;

	CameraInfo cam_old(camera);
	CameraInfo cam_new = cam_old;

	if (!mStyle.mFocusROI.isEmpty())
	{
		DoubleBoundingBox3D roi_r = this->getROI(mStyle.mFocusROI).getBox();
		if (roi_r != DoubleBoundingBox3D::zero())
			cam_new.focus = roi_r.center();
	}

	if (mFollowingTool)
	{
		Transform3D rMpr = mBackend->patient()->get_rMpr();
		Transform3D prMt = mFollowingTool->get_prMt();
		Transform3D rMt = rMpr * prMt;
		double offset = mFollowingTool->getTooltipOffset();

		// view up is relative to tool
		cam_new.vup = rMt.vector(Vector3D(-1, 0, 0));

		if (mStyle.mFocusFollowTool)
		{
			// set focus to tool offset point
			cam_new.focus = rMt.coord(Vector3D(0, 0, offset));
		}

		if (mStyle.mCameraFollowTool)
		{
			// set camera on the tool line, keeping the previous distance from the focus.
			Vector3D tooloffset = rMt.coord(Vector3D(0, 0, offset));
			Vector3D e_tool = rMt.vector(Vector3D(0, 0, 1));
			cam_new.pos = NavigationAlgorithms::findCameraPosOnLineFixedDistanceFromFocus(tooloffset,
																					   e_tool,
																					   cam_old.distance(),
																					   cam_new.focus);
		}
	}

	if (mStyle.mTableLock)
	{
		Vector3D table_up = mBackend->patient()->getOperatingTable().getVectorUp();
		cam_new.vup = table_up;
	}

	if (mStyle.mCameraFollowTool)
	{
		cam_new.pos = NavigationAlgorithms::elevateCamera(mStyle.mElevation, cam_new.pos, cam_new.focus, cam_new.vup);
	}

	// reset vup based on vpn (do not change vpn after this point)
	if (!similar(cam_new.vup, cam_old.vup))
	{
		cam_new.vup = NavigationAlgorithms::orthogonalize_vup(cam_new.vup, cam_new.vpn(), cam_old.vup);
	}

	if (!mStyle.mAutoZoomROI.isEmpty())
	{
		cam_new = this->viewEntireAutoZoomROI(cam_new);
	}

//	if (mStyle.mCameraOnTooltip && mFollowingTool)
//	{
//		// Move the camera onto the tool tip, keeping the distance vector constant.
//		// This gives the effect of _sitting on the tool tip_ while moving.
//		// Alternative: Dont change focal point, change view angle instead.
//		Vector3D delta = this->getToolTip_r() - cam_new.pos;
//		cam_new.pos += delta;
//		cam_new.focus += delta;
//	}

	this->updateCamera(cam_new);
}

Vector3D CameraStyleForView::getToolTip_r()
{
	Transform3D rMpr = mBackend->patient()->get_rMpr();
	Transform3D prMt = mFollowingTool->get_prMt();
	Transform3D rMt = rMpr * prMt;
	double offset = mFollowingTool->getTooltipOffset();
	Vector3D tool_r = rMt.coord(Vector3D(0, 0, offset));
	return tool_r;
}

void debugPrint(CameraInfo info)
{
	CX_LOG_CHANNEL_DEBUG("CA") << "  info.focus="<<info.focus;
	CX_LOG_CHANNEL_DEBUG("CA") << "  info.pos="<<info.pos;
	CX_LOG_CHANNEL_DEBUG("CA") << "  info.vup="<<info.vup;
	CX_LOG_CHANNEL_DEBUG("CA") << "  info.vpn() ="<<info.vpn();
	CX_LOG_CHANNEL_DEBUG("CA") << "  info.distance() ="<<info.distance();
}



CameraInfo CameraStyleForView::viewEntireAutoZoomROI(CameraInfo info)
{
	CameraInfo retval = info;
	if (mStyle.mAutoZoomROI.isEmpty())
		return retval;

	RegionOfInterest roi_r = this->getROI(mStyle.mAutoZoomROI);
	if (!roi_r.isValid())
		return retval;

	double viewAngle = info.viewAngle/180.0*M_PI;

	this->getRenderer()->ComputeAspect();
	double aspect[2];
	this->getRenderer()->GetAspect(aspect);

	double viewAngle_vertical = viewAngle;
	double viewAngle_horizontal = viewAngle*aspect[0];

	// move all calculations into a space p: (x,y,c)=(left,vup,focus)
	// used to define a ROI bounding box aligned to the viewport.
	Vector3D left = cross(info.vup, info.vpn());
	Transform3D pMr = createTransformIJC(left, info.vup, info.focus).inv();
	CameraInfo cam_proj;
	cam_proj.focus = pMr.coord(info.focus);
	cam_proj.vup = pMr.vector(info.vup);
	cam_proj.pos = pMr.coord(info.pos);

	DoubleBoundingBox3D proj_bb = roi_r.getBox(pMr);

	Vector3D dist = cam_proj.focus - cam_proj.pos;
	cam_proj.pos = NavigationAlgorithms::findCameraPosByZoomingToROI(viewAngle_vertical,
																	 viewAngle_horizontal,
																	 cam_proj.focus,
																	 cam_proj.vup,
																	 cam_proj.vpn(),
																	 proj_bb);
	// keep focus at a const distance in front of the camera (if we dont do this, vpn might swap)
	cam_proj.focus = (cam_proj.pos + dist).normal() * 100;

	cam_proj.pos = this->smoothZoomedCameraPosition(cam_proj.pos);

	retval.pos = pMr.inv().coord(cam_proj.pos);
	retval.focus = pMr.inv().coord(cam_proj.focus);

	// experimental:
	// IF inside bb: move pos to tool tip
	// IF in front of bb: do nothing.
	// IF in bbx2: interpolate between the two
	// IF behind bb: keep pos inside bb
	if (mStyle.mCameraOnTooltip && mFollowingTool)
	{
		CX_LOG_CHANNEL_DEBUG("CA") << "";
		Vector3D proj_tool = pMr.coord(this->getToolTip_r());
		Vector3D e_z(0,0,1);
		double tool_z = dot(proj_tool, e_z);
		double bb_min_z = proj_bb[4];
		double bb_max_z = proj_bb[5];
		double bb_ext_z = proj_bb[5] + 50;
		double bb_extension = 50; // distance from bb where we want to interpolate between on-tool and off-tool
		CX_LOG_CHANNEL_DEBUG("CA") << "  tool_z="<<tool_z
								   <<", bb_min_z="<<bb_min_z
								   <<", bb_max_z="<<bb_max_z
								   <<", bb_ext_z="<<bb_ext_z;

		Vector3D new_pos;
		if (tool_z < bb_min_z)
		{
			// behind roi: lock camera pos to closest pos inside bb
			new_pos = proj_tool + (bb_min_z-tool_z)*e_z;
			CX_LOG_CHANNEL_DEBUG("CA") << "  **  behind roi";
		}
		else
		{
			double s = (tool_z-bb_max_z)/bb_extension;
			s = std::min(1.0, s);
			s = std::max(0.0, s);
			new_pos = (1.0-s)*proj_tool + (s)*cam_proj.pos;
			CX_LOG_CHANNEL_DEBUG("CA") << "  **  roi pos= s=" << s;
		}

		// Move the camera onto the tool tip, keeping the distance vector constant.
		// This gives the effect of _sitting on the tool tip_ while moving.
		// Alternative: Dont change focal point, change view angle instead.
		Vector3D delta = pMr.inv().coord(new_pos) - retval.pos;
		retval.pos += delta;
		retval.focus += delta;
	}

	return retval;
}

void CameraStyleForView::handleLights()
{
	vtkRendererPtr renderer = this->getRenderer();
//	CX_LOG_CHANNEL_DEBUG("CA") << "#lights: " << renderer->GetLights()->GetNumberOfItems();
	renderer->GetLights()->InitTraversal();
	vtkLight* light = renderer->GetLights()->GetNextItem();
//	CX_LOG_CHANNEL_DEBUG("CA") << "light ";
//	light->PrintSelf(std::cout, vtkIndent(2));

	// experiment: set a light to the left of the camera, pointing at focus
	light->SetConeAngle(160);
	light->SetLightTypeToCameraLight();
	light->SetPosition(-0.5,0,1);
}

/**
 * Remove jitter on the camera position: find the previous position, then stick to it
 * until a threshold is exceeded.
 */
Vector3D CameraStyleForView::smoothZoomedCameraPosition(Vector3D pos)
{
	Vector3D filteredPos = pos;
	filteredPos[2] = mZoomJitterFilter.newValue(pos[2]);
	return filteredPos;
}

RegionOfInterest CameraStyleForView::getROI(QString uid) const
{
	DataPtr data = mBackend->patient()->getData(uid);
	RegionOfInterestMetricPtr roi = boost::dynamic_pointer_cast<RegionOfInterestMetric>(data);
	if (roi)
		return roi->getROI();
	return RegionOfInterest();
}

void CameraStyleForView::activeToolChangedSlot()
{
	ToolPtr newTool = mBackend->tracking()->getActiveTool();
	if (newTool == mFollowingTool)
		return;

	this->disconnectTool();
	this->connectTool();
}

bool CameraStyleForView::isToolFollowingStyle() const
{
	return (mStyle.mCameraFollowTool || mStyle.mFocusFollowTool);
}

void CameraStyleForView::connectTool()
{
	if (!this->isToolFollowingStyle())
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
		rep->setOffsetPointVisibleAtZeroOffset(false);
		if (mStyle.mCameraFollowTool && fabs(mStyle.mElevation) < 0.01)
			rep->setStayHiddenAfterVisible(true);
	}

	this->setModified();

	report("Camera is following " + mFollowingTool->getName());
}

void CameraStyleForView::disconnectTool()
{
	if (mFollowingTool)
	{
		disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(setModified()));

		ToolRep3DPtr rep = this->getToolRep();
		if (rep)
		{
			rep->setOffsetPointVisibleAtZeroOffset(true);
			rep->setStayHiddenAfterVisible(false);
		}

		mFollowingTool.reset();
	}
}

void CameraStyleForView::setCameraStyle(CameraStyleData style)
{
	if (mStyle == style)
		return;

	this->disconnectTool();

	if (style.mUniCam)
		this->setInteractor(vtkInteractorStyleUnicamPtr::New());
	else
		this->setInteractor(vtkInteractorStyleTrackballCameraPtr::New());

	mStyle = style;

	this->connectTool();
}

void CameraStyleForView::setInteractor(vtkSmartPointer<vtkInteractorStyle> style)
{
	ViewPtr view = this->getView();
	if (!view)
		return;
	vtkRenderWindowInteractor* interactor = view->getRenderWindow()->GetInteractor();
	interactor->SetInteractorStyle(style);
}

CameraStyleData CameraStyleForView::getCameraStyle()
{
	return mStyle;
}

void CameraStyleForView::updateCamera(CameraInfo info)
{
	vtkCameraPtr camera = this->getCamera();
	if (!camera)
		return;
	CameraInfo cam_old(camera);

	// When viewing a scene of 10mm size, errors of 0.01mm are noticeable.
	// Keep tolerance well below this level:
	double tol = 0.001;
	if (similar(cam_old, info, tol))
		return; // break update loop: this event is triggered by camera change.

//	this->handleLights();

	mBlockCameraUpdate = true;
	camera->SetPosition(info.pos.begin());
	camera->SetFocalPoint(info.focus.begin());
	camera->SetViewUp(info.vup.begin());
	camera->SetClippingRange(1, std::max<double>(1000, info.distance() * 10));
	if (mStyle.mCameraFollowTool && mFollowingTool)
		camera->SetClippingRange(1, std::max<double>(1000, info.distance() * 1.5));
	mBlockCameraUpdate = false;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

JitterFilter::JitterFilter()
{
	currentValue = 0;
	range = DoubleRange(0,0,0.1);
}

double JitterFilter::newValue(double value)
{
	// If outside range:
	// reset interval and return value.
	if (( value<=range.min() )||( value>=range.max() ))
	{
		double minimumInterval = 5.0;
		double interval = std::min(minimumInterval, value/20);
		double level = 0;

		if (value<range.min())
			level = value + interval/2;
		else if (value>range.max())
			level = value - interval/2;

		range = DoubleRange(level-interval/2, level+interval/2, interval/10);

		currentValue = value;
	}
	return currentValue;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

CameraInfo::CameraInfo(vtkCameraPtr camera)
{
	pos = Vector3D(camera->GetPosition());
	focus = Vector3D(camera->GetFocalPoint());
	vup = Vector3D(camera->GetViewUp());
	viewAngle = camera->GetViewAngle();
}

bool similar(const CameraInfo &lhs, const CameraInfo &rhs, double tol)
{
	return (similar(lhs.pos, rhs.pos, tol) &&
			similar(lhs.focus, rhs.focus, tol) &&
			similar(lhs.vup, rhs.vup, tol) &&
			similar(lhs.viewAngle, rhs.viewAngle, tol)
			);
}

}//namespace cx
