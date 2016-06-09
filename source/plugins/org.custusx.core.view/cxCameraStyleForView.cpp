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
#include "cxRegionOfInterestMetric.h"

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

	connect(mBackend->tracking().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(activeToolChangedSlot()));
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
//	if (mFollowingTool)
//		this->moveCameraToolStyleSlot();
//	this->moveCameraDefaultStyle();
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

	double cameraOffset = camera->GetDistance();
	Vector3D pos_old(camera->GetPosition());
	Vector3D focus_old(camera->GetFocalPoint());
	Vector3D vup_old(camera->GetViewUp());

	Vector3D camera_r = pos_old;
	Vector3D focus_r = focus_old;
	Vector3D vup_r = vup_old;

	if (mFollowingTool)
	{
		Transform3D rMpr = mBackend->patient()->get_rMpr();
		Transform3D prMt = mFollowingTool->get_prMt();
		Transform3D rMt = rMpr * prMt;
		double offset = mFollowingTool->getTooltipOffset();

		// view up is relative to tool
		vup_r = rMt.vector(Vector3D(-1, 0, 0));

		if (mStyle.mFocusFollowTool)
		{
			// set focus to tool offset point
			focus_r = rMt.coord(Vector3D(0, 0, offset));
		}

		if (mStyle.mCameraFollowTool)
		{
//			// Set camera on line from focus point to tool offset point,
//			Vector3D tooloffset = rMt.coord(Vector3D(0, 0, offset));
//			Vector3D vpn = tooloffset - focus_r;
//			// Solve zero line cases by using tool line
//			if (vpn.length()<0.01)
//				vpn = rMt.vector(Vector3D(0,0,-1));
//			vpn = vpn.normal();
//			// ... at a distance cameraOffset from the focus point.

			// set camera on the tool line, at a distance 'cameraOffset' from the focus.
			Vector3D tooloffset = rMt.coord(Vector3D(0, 0, offset));
			Vector3D e_tool = rMt.vector(Vector3D(0, 0, 1));
			camera_r = this->findCameraPosOnLineFixedDistanceFromFocus(tooloffset, e_tool, cameraOffset, focus_r);

//			camera_r = focus_r + cameraOffset * vpn;
		}
	}

	if (mStyle.mTableLock)
	{
		Vector3D table_up = mBackend->patient()->getOperatingTable().getVectorUp();
		vup_r = table_up;
	}

	if (mStyle.mCameraFollowTool)
		camera_r = this->elevateCamera(mStyle.mElevation, camera_r, focus_r, vup_r);

//	std::cout << "  pos " << pos_old << " to " << camera_r << std::endl;
//	std::cout << "  foc " << focus_old << " to " << focus_r << std::endl;
//	std::cout << "  vpn " << (camera_r-focus_r).normal() << std::endl;
//	std::cout << "  vup " << vup_old << " to " << vup_r << std::endl;

	// reset vup based on vpn
	if (!similar(vup_r, vup_old))
	{
		Vector3D vpn_r = (camera_r-focus_r).normal();
		vup_r = this->orthogonalize_vup(vup_r, vpn_r, vup_old);
	}

	if (!mStyle.mAutoZoomROI.isEmpty())
	{
		DoubleBoundingBox3D roi_r = this->getROI();
		if (roi_r != DoubleBoundingBox3D::zero())
		{
			double viewAngle = camera->GetViewAngle()/180.0*M_PI;
			Vector3D vpn = (camera_r-focus_r).normal();

//			Transform3D rMpr = mBackend->patient()->get_rMpr();
//			Transform3D prMt = mBackend->tracking()->getActiveTool()->get_prMt();
//			Transform3D rMt = rMpr * prMt;
//			Vector3D tp = rMt.coord(Vector3D(0, 0, 0));

//			double t_dist = this->findCameraDistance(viewAngle, focus_r, vpn, tp);

			this->getRenderer()->ComputeAspect();
			double aspect[2];
			this->getRenderer()->GetAspect(aspect);

//			double angle = viewAngle;
//			if(aspect[0]>=1.0) // horizontal window, deal with vertical angle|scale
//			  {
//			  if(this->getCamera()->GetUseHorizontalViewAngle())
//				{
//				angle=2.0*atan(tan(angle*0.5)/aspect[0]);
//				}
//			  }
//			else // vertical window, deal with horizontal angle|scale
//			  {
//			  if(!this->getCamera()->GetUseHorizontalViewAngle())
//				{
//				angle=2.0*atan(tan(angle*0.5)*aspect[0]);
//				}

////			  parallelScale=parallelScale/aspect[0];
//			  }
//			std::cout << "      apect+angle" << aspect[0] << " " << aspect[1] << " " << angle << std::endl;
////			viewAngle = angle;

			double viewAngle_vertical = viewAngle;
			double viewAngle_horizontal = viewAngle*aspect[0];

			Vector3D n_vertical(1, 0, 0);
			double dist_v = this->findMaxCameraDistance(n_vertical, viewAngle_vertical, focus_r, vpn, roi_r);
			Vector3D n_horizontal(0, 1, 0);
			double dist_h = this->findMaxCameraDistance(n_horizontal, viewAngle_horizontal, focus_r, vpn, roi_r);
			double dist = std::max(dist_v, dist_h);


//			Vector3D camera_r_t = focus_r + vpn*t_dist;
			Vector3D camera_r_t = focus_r + vpn*dist;
//					std::cout << "      roi_r: <" << roi_r << std::endl;
//					std::cout << "      camera_r: < " << camera_r << " >" << std::endl;
//					std::cout << "      vpn: < " << vpn << " >" << std::endl;
//					std::cout << "      focus_r: < " << focus_r << " >" << std::endl;
//					std::cout << "      dist: < " << dist << " >" << std::endl;
//					std::cout << "      t_dist: < " << dist << " >" << std::endl;
//					std::cout << "      camera_r_t: < " << camera_r_t << " >" << std::endl;
//					std::cout << "      "  << std::endl;
			camera_r = camera_r_t;
		}
	}

//	CX_LOG_CHANNEL_DEBUG("CA") << " pos " << Vector3D(camera->GetPosition());
//	CX_LOG_CHANNEL_DEBUG("CA") << " foc " << Vector3D(camera->GetFocalPoint());
//	CX_LOG_CHANNEL_DEBUG("CA") << " vup " << Vector3D(camera->GetViewUp());
//	CX_LOG_CHANNEL_DEBUG("CA") << " vpn " << Vector3D(camera->GetViewPlaneNormal());
//	CX_LOG_CHANNEL_DEBUG("CA") << "view angle " << camera->GetViewAngle();
//	CX_LOG_CHANNEL_DEBUG("CA") << "";

	if (similar(pos_old, camera_r, 0.1) && similar(focus_old, focus_r, 0.1) && similar(vup_old, vup_r,0.1 ))
		return; // break update loop: this event is triggered by camera change.

	mBlockCameraUpdate = true;
	camera->SetPosition(camera_r.begin());
	camera->SetFocalPoint(focus_r.begin());
	camera->SetViewUp(vup_r.begin());
	camera->SetClippingRange(1, std::max<double>(1000, cameraOffset * 10));
	if (mStyle.mCameraFollowTool && mFollowingTool)
		camera->SetClippingRange(1, std::max<double>(1000, cameraOffset * 1.5));
	mBlockCameraUpdate = false;

//	CX_LOG_CHANNEL_DEBUG("CA") << "end pos " << Vector3D(camera->GetPosition());
//	CX_LOG_CHANNEL_DEBUG("CA") << "end foc " << Vector3D(camera->GetFocalPoint());
//	CX_LOG_CHANNEL_DEBUG("CA") << "end vup " << Vector3D(camera->GetViewUp());
//	CX_LOG_CHANNEL_DEBUG("CA") << "end vpn " << Vector3D(camera->GetViewPlaneNormal());
//	CX_LOG_CHANNEL_DEBUG("CA") << "view angle " << camera->GetViewAngle();
//	CX_LOG_CHANNEL_DEBUG("CA") << "";
}

Vector3D CameraStyleForView::findCameraPosOnLineFixedDistanceFromFocus(Vector3D p_line, Vector3D e_line, double distance, Vector3D focus)
{
	e_line = e_line.normal();

	// project focus onto line, then use pythoagoras:
	//      dist^2 = |focus-pff|^2 + q^2
	// where we want to find q, dist from p_ff to camera along line.
	Vector3D p_ff = p_line + e_line * dot(focus-p_line, e_line);
	double q_sq = distance*distance - dot(focus-p_ff, focus-p_ff);
	if (q_sq<0)
	{
		// too close: revert to use point on p_ff - focus line.
		Vector3D p_c = focus - (p_ff-focus).normal() * distance;
		return p_c;
	}
	double q = sqrt(q_sq);

	Vector3D p_c = p_ff - q * e_line;
	Vector3D p2_c = p_ff + q * e_line;

	if (similar((p_c-focus).length(), distance))
		return p_c;
	else if (similar((p2_c-focus).length(), distance))
	{
		return p2_c;
	}
	else
	{
		CX_LOG_CHANNEL_DEBUG("CA") << "find point failed - error in distance!!!!!!!!!!!!!!!!!";
		return p_c;
	}
}

double CameraStyleForView::findMaxCameraDistance(Vector3D n, double viewAngle, Vector3D focus, Vector3D vpn, const DoubleBoundingBox3D& bb)
{
	std::vector<double> dists;
	for (unsigned x=0; x<2; ++x)
		for (unsigned y=0; y<2; ++y)
			for (unsigned z=0; z<2; ++z)
			{
				Vector3D p = bb.corner(x,y,z);
				double d = this->findCameraDistanceKeepPointInViewOneAxis(n, viewAngle, focus, vpn, p);
				dists.push_back(d);
			}

	double maxDist = *std::max_element(dists.begin(), dists.end());
	return maxDist;
}

/**
 * Find the camera distance required to keep p in view,
 * give a viewangle, focus and vpn.
 * The resulting distance is the minimum camera distance required to see p.
 *
 */
double CameraStyleForView::findCameraDistanceKeepPointInViewOneAxis(Vector3D n, double viewAngle, Vector3D focus, Vector3D vpn, Vector3D p)
{
	// project all items into plane	n
	Vector3D focus_p = focus - dot(focus, n)*n;
	Vector3D p_p = p - dot(p, n)*n;
	Vector3D vpn_p = (vpn - dot(vpn, n)*n).normal();

	// find distance in projection plane n
	double d_p = this->findCameraDistanceKeepPointInView(viewAngle, focus_p, vpn_p, p_p);

	// recalculate non-projected distance.
	double cos_plane_angle = dot(vpn, vpn_p); // cosine(angle) between plane n and original vpn direction
	d_p = d_p / cos_plane_angle;
	return d_p;
}

/**
 * Find the camera distance required to keep p in view,
 * give a viewangle, focus and vpn.
 * The resulting distance is the minimum camera distance required to see p.
 *
 */
double CameraStyleForView::findCameraDistanceKeepPointInView(double viewAngle, Vector3D focus, Vector3D vpn, Vector3D p)
{
	Vector3D pp = focus + vpn*dot(p-focus, vpn); // p projected onto the camera line defined by focus and vpn.
	double beta = (p-pp).length() / tan(viewAngle/2); // distance from pp to camera
	beta = fabs(beta);
	double dist = beta + dot(pp-focus, vpn); // total distance from focus to camera
	return dist;
}

DoubleBoundingBox3D CameraStyleForView::getROI()
{
	DataPtr data = mBackend->patient()->getData(mStyle.mAutoZoomROI);
	RegionOfInterestMetricPtr roi = boost::dynamic_pointer_cast<RegionOfInterestMetric>(data);
	if (roi)
		return roi->getROI();
	return DoubleBoundingBox3D::zero();
}

//DoubleBoundingBox3D CameraStyleForView::getROI()
//{
//	// create a dummy ROI containing vol center and tool plus margin
//	std::map<QString, DataPtr> alldata = mBackend->patient()->getData();
//	std::map<QString, DataPtr> data;
//	// spike: add one data
//	data["point1"] = alldata["point1"];

//	std::vector<Vector3D> points;
//	// create a max ROI containing all data plus margin
//	for (std::map<QString, DataPtr>::const_iterator i=data.begin(); i!=data.end(); ++i)
//	{
//		std::vector<Vector3D> c = this->getCorners_r(i->second);
//		std::copy(c.begin(), c.end(), back_inserter(points));
//	}

//	if (mFollowingTool)
//	{
//		Transform3D rMpr = mBackend->patient()->get_rMpr();
//		Transform3D prMt = mFollowingTool->get_prMt();
//		Transform3D rMt = rMpr * prMt;
//		double offset = mFollowingTool->getTooltipOffset();
//		Vector3D tp = rMt.coord(Vector3D(0, 0, offset));
//		points.push_back(tp);
//	}

//	double margin = 20;
//	DoubleBoundingBox3D bb = this->generateROIFromPointsAndMargin(points, margin);
//	return bb;
//}

//DoubleBoundingBox3D CameraStyleForView::getMaxROI()
//{
//	std::map<QString, DataPtr> alldata = mBackend->patient()->getData();
//	if (alldata.empty())
//		return DoubleBoundingBox3D::zero();
//	std::vector<Vector3D> points;
//	// create a max ROI containing all data plus margin
//	for (std::map<QString, DataPtr>::const_iterator i=alldata.begin(); i!=alldata.end(); ++i)
//	{
//		std::vector<Vector3D> c = this->getCorners_r(i->second);
//		std::copy(c.begin(), c.end(), back_inserter(points));
//	}

////	if (mFollowingTool)
//	{
//		Transform3D rMpr = mBackend->patient()->get_rMpr();
//		Transform3D prMt = mBackend->tracking()->getActiveTool()->get_prMt();
//		Transform3D rMt = rMpr * prMt;
//		double offset = 0;
//		Vector3D tp = rMt.coord(Vector3D(0, 0, offset));
//		points.push_back(tp);
//	}


//	double margin = 20;
//	DoubleBoundingBox3D bb = this->generateROIFromPointsAndMargin(points, margin);
//	return bb;
//}

//DoubleBoundingBox3D CameraStyleForView::generateROIFromPointsAndMargin(const std::vector<Vector3D>& points, double margin)
//{
//	DoubleBoundingBox3D bb = DoubleBoundingBox3D::fromCloud(points);
//	Vector3D vmargin(margin,margin, margin);
//	Vector3D bl = bb.bottomLeft() - vmargin;
//	Vector3D tr = bb.topRight() + vmargin;
//	bb = DoubleBoundingBox3D(bl, tr);

//	return bb;
//}

//std::vector<Vector3D> CameraStyleForView::getCorners_r(DataPtr data)
//{
//	DoubleBoundingBox3D bb = data->boundingBox();
//	std::vector<Vector3D> retval;

//	for (unsigned x=0; x<2; ++x)
//		for (unsigned y=0; y<2; ++y)
//			for (unsigned z=0; z<2; ++z)
//				retval.push_back(bb.corner(x,y,z));

//	for (unsigned i=0; i<retval.size(); ++i)
//		retval[i] = data->get_rMd().coord(retval[i]);

//	return retval;
//}

/**
 * find a vup orthogonal to vpn
 */
Vector3D CameraStyleForView::orthogonalize_vup(Vector3D vup, Vector3D vpn, Vector3D vup_fallback)
{
	if (cross(vup, vpn).length() < 0.01)
	{
		CX_LOG_CHANNEL_DEBUG("CA") << "warning ,  cross(vup_r, vpn_r)=" << cross(vup, vpn).length();
		vup = vup_fallback;
	}
	else
	{
		Vector3D left = cross(vup, vpn).normal();
//		CX_LOG_CHANNEL_DEBUG("CA") << "left " << cross(vup_r, vpn_r);
		vup = cross(vpn, left).normal();
	}

	return vup;
}


/**
 * Elevate the camera position a given angle towards vup.
 */
Vector3D CameraStyleForView::elevateCamera(double angle, Vector3D camera, Vector3D focus, Vector3D vup)
{
	if (similar(angle, 0.0))
		return camera;

	double cameraOffset = (camera-focus).length();
	// elevate, but keep distance
	double height = cameraOffset * tan(angle);
	camera += vup * height;
	Vector3D elevated = camera + vup * height;
	Vector3D n_foc2eye = (elevated - focus).normalized();
	camera = focus + cameraOffset * n_foc2eye;
	return camera;
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

	ViewPtr view = this->getView();
	if (!view)
		return;
	vtkRenderWindowInteractor* interactor = view->getRenderWindow()->GetInteractor();

	if (style.mUniCam)
	{
		interactor->SetInteractorStyle(vtkInteractorStyleUnicamPtr::New());
	}
	else
	{
		interactor->SetInteractorStyle(vtkInteractorStyleTrackballCameraPtr::New());
	}

	mStyle = style;

	this->connectTool();
}

CameraStyleData CameraStyleForView::getCameraStyle()
{
	return mStyle;
}

}//namespace cx
