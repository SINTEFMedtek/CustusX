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


#include "cxSliceProxy.h"

#include <math.h>
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxSliceComputer.h"
#include "cxTool.h"

namespace cx
{

SliceProxyPtr SliceProxy::create(PatientModelServicePtr dataManager)
{
	SliceProxyPtr retval(new SliceProxy(dataManager));
	return retval;
}

SliceProxy::SliceProxy(PatientModelServicePtr dataManager) :
	mCutplane(new SliceComputer())
{
	mDataManager = dataManager;
	mAlwaysUseDefaultCenter = false;
	mUseTooltipOffset = true;
	connect(mDataManager.get(), SIGNAL(centerChanged()),this, SLOT(centerChangedSlot()) ) ;
	connect(mDataManager.get(), SIGNAL(clinicalApplicationChanged()), this, SLOT(clinicalApplicationChangedSlot()));
	//TODO connect to toolmanager rMpr changed
	mDefaultCenter = mDataManager->getCenter();
	this->centerChangedSlot();

	this->initCutplane();
}

SliceProxy::~SliceProxy()
{
}

void SliceProxy::initCutplane()
{
	mCutplane->setFixedCenter(mDefaultCenter);
	mCutplane->setToolPosition(getSyntheticToolPos(mDefaultCenter));
}

void SliceProxy::setTool(ToolPtr tool)
{
	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
		disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(changed())); 
	}

	mTool = tool;

	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
		connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(changed()));/// not used here, but forwarded to users

		emit toolVisible(mTool->getVisible());
		toolTransformAndTimestampSlot(mTool->get_prMt(), 0); // initial values
		tooltipOffsetSlot(mTool->getTooltipOffset());
	}

	this->centerChangedSlot(); // force center update for tool==0
	this->changed();
}

void SliceProxy::toolTransformAndTimestampSlot(Transform3D prMt, double timestamp)
{
	//std::cout << "proxy get transform" << std::endl;
	Transform3D rMpr = mDataManager->get_rMpr();
	Transform3D rMt = rMpr*prMt;
//	if (similar(rMt, mCutplane->getToolPosition()))
//	{
//    return;
//	}
	mCutplane->setToolPosition(rMt);
	this->changed();
	emit toolTransformAndTimestamp(prMt, timestamp);

}

void SliceProxy::tooltipOffsetSlot(double val)
{
	if (mUseTooltipOffset)
	{
		mCutplane->setToolOffset(val);
		this->changed();
	}
}

void SliceProxy::setUseTooltipOffset(bool use)
{
	if (!use)
	{
		tooltipOffsetSlot(0);
	}
	mUseTooltipOffset = use;
	if (use)
	{
		tooltipOffsetSlot(mTool->getTooltipOffset());
	}
}

void SliceProxy::toolVisibleSlot(bool visible)
{

}

/**Provide a nice default transform for displays without a tool.
 */
Transform3D SliceProxy::getSyntheticToolPos(const Vector3D& center) const
{
	Transform3D R_tq = createTransformRotateY(M_PI) * createTransformRotateZ(M_PI_2);
	Transform3D T_c = createTransformTranslate(center);
	return T_c * R_tq;
}

void SliceProxy::setDefaultCenter(const Vector3D& c)
{
	mDefaultCenter = c;
	this->centerChangedSlot();
}

void SliceProxy::setAlwaysUseDefaultCenter(bool on)
{
	mAlwaysUseDefaultCenter = on;
	this->centerChangedSlot();
}

void SliceProxy::centerChangedSlot()
{
	if (mAlwaysUseDefaultCenter)
	{
		mCutplane->setFixedCenter(mDefaultCenter);
	}
	else if (mTool)
	{
		Vector3D c = mDataManager->getCenter();
		mCutplane->setFixedCenter(c);
		//std::cout << "center changed: " + string_cast(c) << std::endl;
	}

	changed();
}

void SliceProxy::clinicalApplicationChangedSlot()
{
	mCutplane->setClinicalApplication(mDataManager->getClinicalApplication());
	changed();
}

/**Group the typical plane definition uses together.
 */
void SliceProxy::initializeFromPlane(PLANE_TYPE plane, bool useGravity, const Vector3D& gravityDir, bool useViewOffset, double viewportHeight, double toolViewOffset, bool useConstrainedViewOffset)
{
	mCutplane->initializeFromPlane(plane,
								   useGravity, gravityDir,
								   useViewOffset, viewportHeight, toolViewOffset,
								   mDataManager->getClinicalApplication(),
								   useConstrainedViewOffset);
	changed();
//	setPlane(plane);
//	//Logger::log("vm.log"," set plane to proxy ");
//	if (plane == ptSAGITTAL || plane == ptCORONAL || plane == ptAXIAL )
//	{
//		setOrientation(otORTHOGONAL);
//		setFollowType(ftFIXED_CENTER);
//	}
//	else if (plane == ptANYPLANE || plane==ptRADIALPLANE || plane==ptSIDEPLANE)
//	{
//		setOrientation(otOBLIQUE);
//		setFollowType(ftFOLLOW_TOOL);
//
//		setGravity(useGravity, gravityDir);
//		setToolViewOffset(useViewOffset, viewportHeight, toolViewOffset); // TODO finish this one
//	}
}

SliceComputer SliceProxy::getComputer() const
{
	return *mCutplane;
}

void SliceProxy::setComputer(const SliceComputer& val)
{
	mCutplane.reset(new SliceComputer(val));
	changed();
}

void SliceProxy::setOrientation(ORIENTATION_TYPE orientation)
{
	mCutplane->setOrientationType(orientation);
	changed();
}

void SliceProxy::setPlane(PLANE_TYPE plane)
{
	mCutplane->setPlaneType(plane);
	changed();
}

void SliceProxy::setFollowType(FOLLOW_TYPE followType)
{
	mCutplane->setFollowType(followType);
	changed();
}

void SliceProxy::setGravity(bool use, const Vector3D& dir)
{
	mCutplane->setGravity(use, dir);
	this->changed();
}
void SliceProxy::setToolViewOffset(bool use, double viewportHeight, double toolViewOffset, bool useConstrainedViewOffset)
{
	mCutplane->setToolViewOffset(use, viewportHeight, toolViewOffset, useConstrainedViewOffset);
	this->changed();
}
 
void SliceProxy::setToolViewportHeight(double viewportHeight)
{
	mCutplane->setToolViewportHeight(viewportHeight);
	this->changed();
}

ToolPtr SliceProxy::getTool()
{
	return mTool;
}

Transform3D SliceProxy::get_sMr()
{
	SlicePlane plane = mCutplane->getPlane();
	//std::cout << "---" << " proxy get transform.c : " << plane.c << std::endl;
	//std::cout << "proxy get transform -" << getName() <<":\n" << plane << std::endl;
	return createTransformIJC(plane.i, plane.j, plane.c).inv();
}

void SliceProxy::changed()
{
	emit transformChanged(get_sMr());
}

void SliceProxy::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "sliceproxy" << std::endl;
	os << indent << "sMr: " << std::endl;
	get_sMr().put(os, indent.getIndent()+3);
	os << std::endl;
}

} // namespace cx
