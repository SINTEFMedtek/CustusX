/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscSlicePlaneClipper.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "cxSlicePlaneClipper.h"

#include <vector>
#include <vtkPlane.h>

#include "cxSliceProxy.h"

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
	emit slicePlaneChanged();
}

} // namespace cx
