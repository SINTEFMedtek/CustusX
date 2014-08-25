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
