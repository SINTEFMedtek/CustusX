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

#include "cxSliceProxy.h"
#include "cxSettings.h"
#include "cxSliceComputer.h"
#include "cxTool.h"
#include "cxUtilHelpers.h"
#include "cxDefinitionStrings.h"

#include "cxPatientModelService.h"


namespace cx
{

ViewFollowerPtr ViewFollower::create(PatientModelServicePtr dataManager)
{
	return ViewFollowerPtr(new ViewFollower(dataManager));
}

ViewFollower::ViewFollower(PatientModelServicePtr dataManager) :
	mDataManager(dataManager)
{

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

    // views which follow the tool don't need any correction
    if (mSliceProxy->getComputer().getFollowType()==ftFOLLOW_TOOL)
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
	Vector3D c_s = sMr.coord(mDataManager->getCenter());
	Vector3D newcenter_s = c_s + shift_s;
	Vector3D newcenter_r = sMr.inv().coord(newcenter_s);
	mDataManager->setCenter(newcenter_r);
}

} // namespace cx

