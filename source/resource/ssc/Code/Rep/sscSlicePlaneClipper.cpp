// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscSlicePlaneClipper.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "sscSlicePlaneClipper.h"

#include <vector>
#include <vtkPlane.h>

#include "sscSliceProxy.h"

namespace cx
{

SlicePlaneClipperPtr SlicePlaneClipper::New()
{
	return SlicePlaneClipperPtr(new SlicePlaneClipper());
}

SlicePlaneClipper::SlicePlaneClipper() :
	mInvertPlane(false)
{
}

SlicePlaneClipper::~SlicePlaneClipper()
{
}

void SlicePlaneClipper::setSlicer(SliceProxyPtr slicer)
{
	if (mSlicer==slicer)
		return;
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
	}
	mSlicer = slicer;
	if (mSlicer)
	{
		connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
	}

	this->updateClipPlane();
	this->changedSlot();
}

SliceProxyPtr SlicePlaneClipper::getSlicer()
{
	return mSlicer;
}

void SlicePlaneClipper::setInvertPlane(bool on)
{
	mInvertPlane = on;
	changedSlot();
}

bool SlicePlaneClipper::getInvertPlane() const
{
	return mInvertPlane;
}

/** return an untransformed plane normal to use during clipping.
 *  The direction is dependent in invertedPlane()
 */
Vector3D SlicePlaneClipper::getUnitNormal() const
{
	if (mInvertPlane)
		return Vector3D(0,0,1);
	else
		return Vector3D(0,0,-1);
}

/** return a vtkPlane representing the current clip plane.
 */
vtkPlanePtr SlicePlaneClipper::getClipPlaneCopy()
{
	vtkPlanePtr retval = vtkPlanePtr::New();
	retval->SetNormal(mClipPlane->GetNormal());
	retval->SetOrigin(mClipPlane->GetOrigin());
	return retval;
}

vtkPlanePtr SlicePlaneClipper::getClipPlane()
{
	return mClipPlane;
}

void SlicePlaneClipper::updateClipPlane()
{
	if (!mSlicer)
		return;
	if (!mClipPlane)
		mClipPlane = vtkPlanePtr::New();

	Transform3D rMs = mSlicer->get_sMr().inv();

	Vector3D n = rMs.vector(this->getUnitNormal());
	Vector3D p = rMs.coord(Vector3D(0,0,0));
	mClipPlane->SetNormal(n.begin());
	mClipPlane->SetOrigin(p.begin());
}

void SlicePlaneClipper::changedSlot()
{
	if (!mSlicer)
		return;

	this->updateClipPlane();
}

} // namespace cx
