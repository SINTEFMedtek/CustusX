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

#include "cxViewFollower.h"

#include "sscSliceProxy.h"
#include "cxSettings.h"
#include "sscSliceComputer.h"
#include "sscTool.h"
#include "sscUtilHelpers.h"
#include "sscDefinitionStrings.h"

#include "sscToolManager.h"
#include "sscDataManager.h"
#include "sscMessageManager.h"

namespace cx
{

void ViewFollower::setSliceProxy(SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
	{
		disconnect(mSliceProxy.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(ensureCenterWithinView()));
	}

	mSliceProxy = sliceProxy;

	if (mSliceProxy)
	{
		connect(mSliceProxy.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(ensureCenterWithinView()));
	}
}

void ViewFollower::setView(DoubleBoundingBox3D bb_s)
{
	mBB_s = bb_s;
	this->ensureCenterWithinView();
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
	if (mSliceProxy->getComputer().getOrientationType()!=otORTHOGONAL)
		return;

	Vector3D shift_s = this->findCenterShift_s();
	this->applyShiftToCenter(shift_s);
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
	Transform3D rMpr = dataManager()->get_rMpr();
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

void ViewFollower::applyShiftToCenter(Vector3D shift_s)
{
	Transform3D sMr = mSliceProxy->get_sMr();
	Vector3D c_s = sMr.coord(dataManager()->getCenter());
	Vector3D newcenter_s = c_s + shift_s;
	Vector3D newcenter_r = sMr.inv().coord(newcenter_s);
	dataManager()->setCenter(newcenter_r);
}

} // namespace cx

