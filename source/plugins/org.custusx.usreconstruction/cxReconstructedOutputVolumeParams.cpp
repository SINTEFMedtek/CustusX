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


#include "cxReconstructedOutputVolumeParams.h"

namespace cx
{

OutputVolumeParams::OutputVolumeParams() :
	mInputSpacing(0),
	mMaxVolumeSize(32*1000),
	mValid(false)
{

}
/** Initialize the volue parameters with sensible defaults.
 */
OutputVolumeParams::OutputVolumeParams(DoubleBoundingBox3D extent, double inputSpacing, double maxVolumeSize) :
	mInputSpacing(inputSpacing), mMaxVolumeSize(maxVolumeSize), mValid(false)
{
	mImage.setSpacingKeepDim(Eigen::Array3d(inputSpacing, inputSpacing, inputSpacing));
	mImage.setDimKeepBoundsAlignSpacing(extent.range());

	this->constrainVolumeSize();

	if(mImage.getDim().minCoeff() <= 1) //At least one of the dimensions <= 1
		mValid = false;
	else
		mValid = true;
}

unsigned long OutputVolumeParams::getVolumeSize() const
{
	return mImage.getNumVoxels();
}

/** Set a spacing, recalculate dimensions.
 */
void OutputVolumeParams::setSpacing(double spacing)
{
	this->suggestSpacingKeepBounds(spacing);
}
double OutputVolumeParams::getSpacing() const
{
	return mImage.getSpacing()[0];
}
/** Set one of the dimensions explicitly, recalculate other dims and spacing.
 */
void OutputVolumeParams::setDim(int index, int newDim)
{
	double newSpacing = mImage.getBounds()[index] / (newDim);
	this->setSpacing(newSpacing);
}
Eigen::Array3i OutputVolumeParams::getDim() const
{
	return mImage.getDim();
}
/** Increase spacing in order to keep size below a max size
 */
void OutputVolumeParams::constrainVolumeSize()
{
	this->suggestSpacingKeepBounds(mInputSpacing);
	mImage.limitVoxelsKeepBounds(this->getMaxVolumeSize());
	mImage.changeToUniformSpacing();
}

void OutputVolumeParams::suggestSpacingKeepBounds(double spacing)
{
	Eigen::Array3d bounds = mImage.getBounds();
	mImage.setSpacingKeepDim(Eigen::Array3d(spacing, spacing, spacing));
	mImage.setDimKeepBoundsAlignSpacing(bounds);
}

void OutputVolumeParams::setMaxVolumeSize(double maxSize)
{
	mMaxVolumeSize = maxSize;
}

unsigned long OutputVolumeParams::getMaxVolumeSize()
{
	return mMaxVolumeSize;
}


void OutputVolumeParams::set_rMd(Transform3D rMd)
{
	mImage.m_rMd = rMd;
}

Transform3D OutputVolumeParams::get_rMd()
{
	return mImage.m_rMd;
}


DoubleBoundingBox3D OutputVolumeParams::getExtent()
{
	return DoubleBoundingBox3D(Vector3D::Zero(), mImage.getBounds());
}

double OutputVolumeParams::getInputSpacing()
{
	return mInputSpacing;
}

} // namespace cx
