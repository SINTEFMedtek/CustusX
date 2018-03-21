/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
