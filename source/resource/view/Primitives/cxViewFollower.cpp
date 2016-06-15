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

#include "cxViewFollower.h"

#include <QTimer>
#include "cxSliceProxy.h"
#include "cxSettings.h"
#include "cxSliceComputer.h"
#include "cxTool.h"
#include "cxUtilHelpers.h"
#include "cxDefinitionStrings.h"

#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxRegionOfInterestMetric.h"

namespace cx
{

ViewFollowerPtr ViewFollower::create(PatientModelServicePtr dataManager)
{
	return ViewFollowerPtr(new ViewFollower(dataManager));
}

ViewFollower::ViewFollower(PatientModelServicePtr dataManager) :
	mDataManager(dataManager)
{
//	mROI_s = DoubleBoundingBox3D::zero();
}


void ViewFollower::setSliceProxy(SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
	{
		disconnect(mSliceProxy.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(ensureCenterWithinView()));
	}

	mSliceProxy = sliceProxy;

	if (mSliceProxy)
	{
		connect(mSliceProxy.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(updateView()));
	}
}

void ViewFollower::setView(DoubleBoundingBox3D bb_s)
{
	mBB_s = bb_s;
	this->updateView();
}

void ViewFollower::setAutoZoomROI(QString uid)
{
//	mAutoZoomRoi = uid;

	if (mRoi)
		disconnect(mRoi.get(), &Data::transformChanged, this, &ViewFollower::updateView);

	DataPtr data = mDataManager->getData(uid);
	mRoi = boost::dynamic_pointer_cast<RegionOfInterestMetric>(data);

	if (mRoi)
		connect(mRoi.get(), &Data::transformChanged, this, &ViewFollower::updateView);

	this->updateView();
}

DoubleBoundingBox3D ViewFollower::getROI_BB_s()
{
//	QString roiUid = mAutoZoomROI;
//	DataPtr data = mDataManager->getData(roiUid);
//	RegionOfInterestMetricPtr roi = boost::dynamic_pointer_cast<RegionOfInterestMetric>(data);
	if (!mRoi)
	{
		return DoubleBoundingBox3D::zero();
	}

	CX_LOG_CHANNEL_DEBUG("CA") << "generate bb_roi_s";
	Transform3D sMr = mSliceProxy->get_sMr();
	DoubleBoundingBox3D bb_s = mRoi->getROI().getBox(sMr);
	return bb_s;
}

void ViewFollower::updateView()
{
	QTimer::singleShot(0, this, &ViewFollower::ensureCenterWithinView);
	QTimer::singleShot(0, this, &ViewFollower::autoZoom);
//	this->ensureCenterWithinView();
//	this->autoZoom();
}

void ViewFollower::ensureCenterWithinView()
{
	if (!mSliceProxy)
		return;
	if (!mSliceProxy->getTool())
		return;

	bool followTooltip = settings()->value("Navigation/followTooltip").value<bool>();
	if (!followTooltip)
		return;

	// this applies only to orthogonal views: oblique follows tool anyway
	if (mSliceProxy->getComputer().getOrientationType()==otORTHOGONAL)
	{
		Vector3D shift_s = this->findCenterShift_s();
		this->applyShiftToCenter(shift_s);
	}
}

void ViewFollower::autoZoom()
{
	DoubleBoundingBox3D roi_s = this->getROI_BB_s();
	// autozoom
	// if orthogonal: zoom to centered box
	//                shift pos to box
	//
	// if oblique:    zoom to actual box
	//
	if (mSliceProxy->getComputer().getOrientationType()==otORTHOGONAL)
	{
		// roi: bb defining region of interest
		// box: viewable section
		//
		// find centered roi
		// find zoom needed to see entire centered roi
		// apply zoom to box and emit zoom
		// find shift required to see entire roi
		// set center based on shift

		CX_LOG_CHANNEL_DEBUG("CA") << "";

		Transform3D T = createTransformTranslate(-roi_s.center());
		DoubleBoundingBox3D roi_sc = transform(T, roi_s);
//		CX_LOG_CHANNEL_DEBUG("CA") << "roi_s " << roi_s;
//		CX_LOG_CHANNEL_DEBUG("CA") << "roi_sc " << roi_sc;
//		CX_LOG_CHANNEL_DEBUG("CA") << "mBB_s " << mBB_s;

		double zoom = this->findZoomRequiredToIncludeRoi(mBB_s, roi_sc);
//		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom zoom " << zoom;
		Transform3D S = createTransformScale(Vector3D::Ones()*zoom);
		DoubleBoundingBox3D bb_zoomed = transform(S, mBB_s);
//		CX_LOG_CHANNEL_DEBUG("CA") << "bb_zoomed " << bb_zoomed;

		// find shift
		Vector3D shift = this->findShiftFromBoxToROI(bb_zoomed, roi_s);
//		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom shift " << shift;

		Vector3D newcenter_r = this->findShiftedCenter_r(shift);

		if (!similar(zoom, 1.0))
		{
			CX_LOG_CHANNEL_DEBUG("CA") << this << " autozoom zoom " << zoom;
			emit newZoom(1.0/zoom);
		}
		if (!similar(shift, Vector3D::Zero()))
		{
			CX_LOG_CHANNEL_DEBUG("CA") << this << "autozoom shift " << shift;
			mDataManager->setCenter(newcenter_r);
		}
	}
	else
	{
		// find zoom needed to see entire box
		// emit zoom
		double zoom = this->findZoomRequiredToIncludeRoi(mBB_s, roi_s);
		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom zoom " << zoom;
		emit newZoom(zoom);
	}
}

double ViewFollower::findZoomRequiredToIncludeRoi(DoubleBoundingBox3D base, DoubleBoundingBox3D roi)
{
	double scale = 0;
	// find zoom in x and y
	for (int i=0; i<2; ++i)
	{
		double base_max = fabs(std::max(base[2*i], base[2*i+1]));
		double roi_max = fabs(std::max(roi[2*i], roi[2*i+1]));
		scale = std::max(scale, roi_max/base_max);
	}

	return scale;
}


Vector3D ViewFollower::findCenterShift_s()
{
	Vector3D shift = Vector3D::Zero();

	Vector3D pt_s = this->findVirtualTooltip_s();
	DoubleBoundingBox3D BB_s = this->findStaticBox();
	shift = this->findShiftFromBoxToTool_s(BB_s, pt_s);

//	if (!similar(shift, Vector3D::Zero()))
//	{
//		std::cout << "type: " << enum2string(mSliceProxy->getComputer().getPlaneType()) << std::endl;
//		std::cout << "mBB_s: " << mBB_s << std::endl;
//		std::cout << "BB_s: " << BB_s << std::endl;
//		std::cout << "pt_s: " << pt_s << std::endl;
//		Vector3D pt_r = rMpr * prMt.coord(Vector3D(0,0,tool->getTooltipOffset()));
//		std::cout << "pt_r: " << pt_r << std::endl;
//		std::cout << "shift: " << shift << std::endl;
//	}

	return shift;
}

Vector3D ViewFollower::findVirtualTooltip_s()
{
	ToolPtr tool = mSliceProxy->getTool();
	Transform3D sMr = mSliceProxy->get_sMr();
	Transform3D rMpr = mDataManager->get_rMpr();
	Transform3D prMt = tool->get_prMt();
	Vector3D pt_s = sMr * rMpr * prMt.coord(Vector3D(0,0,tool->getTooltipOffset()));
	pt_s[2] = 0; // project into plane
	return pt_s;
}

DoubleBoundingBox3D ViewFollower::findStaticBox()
{
	double followTooltipBoundary = settings()->value("Navigation/followTooltipBoundary").toDouble();
	followTooltipBoundary = constrainValue(followTooltipBoundary, 0.0, 0.5);
	Transform3D S = createTransformScale(Vector3D::Ones()*(1.0-2.0*followTooltipBoundary));
	Transform3D T = createTransformTranslate(mBB_s.center());
	DoubleBoundingBox3D BB_s = transform(T*S*T.inv(), mBB_s);
	return BB_s;
}

/**
 * Find the shift required to move BB_s to include pt_s.
 */
Vector3D ViewFollower::findShiftFromBoxToTool_s(DoubleBoundingBox3D BB_s, Vector3D pt_s)
{
	Vector3D shift = Vector3D::Zero();

	for (unsigned i=0; i<2; ++i) // loop over two first dimensions, check if pt outside of bb
	{
		if (pt_s[i] < BB_s[2*i])
			shift[i] += pt_s[i] - BB_s[2*i];
		if (pt_s[i] > BB_s[2*i+1])
			shift[i] += pt_s[i] - BB_s[2*i+1];
	}

	return shift;
}

/**
 * Find the shift required to move BB_s to include roi_s.
 */
Vector3D ViewFollower::findShiftFromBoxToROI(DoubleBoundingBox3D bb, DoubleBoundingBox3D roi)
{
	Vector3D shift = Vector3D::Zero();

	for (unsigned i=0; i<2; ++i) // loop over two first dimensions, check if roi outside of bb
	{
		if (roi[2*i  ] < bb[2*i  ])
			shift[i] += roi[2*i  ] - bb[2*i ];
		if (roi[2*i+1] > bb[2*i+1])
			shift[i] += roi[2*i+1] - bb[2*i+1];
	}

//	for (unsigned i=0; i<2; ++i) // loop over two first dimensions, check if roi outside of bb
//	{
//		if (roi_s[2*i+1] < BB_s[2*i])
//			shift[i] += roi_s[2*i+1] - BB_s[2*i];
//		if (roi_s[2*i] > BB_s[2*i+1])
//			shift[i] += roi_s[i] - BB_s[2*i+1];
//	}

	return shift;
}

Vector3D ViewFollower::findShiftedCenter_r(Vector3D shift_s)
{
	Transform3D sMr = mSliceProxy->get_sMr();
	Vector3D c_s = sMr.coord(mDataManager->getCenter());
	Vector3D newcenter_s = c_s + shift_s;
	Vector3D newcenter_r = sMr.inv().coord(newcenter_s);
	return newcenter_r;
}

void ViewFollower::applyShiftToCenter(Vector3D shift_s)
{
	Vector3D newcenter_r = this->findShiftedCenter_r(shift_s);
	mDataManager->setCenter(newcenter_r);
}

} // namespace cx

