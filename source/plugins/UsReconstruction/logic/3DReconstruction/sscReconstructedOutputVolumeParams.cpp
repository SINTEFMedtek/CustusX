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

#include "sscReconstructedOutputVolumeParams.h"

namespace cx
{

OutputVolumeParams::OutputVolumeParams() :
	mInputSpacing(0),
	mMaxVolumeSize(32*1000)
{

}
/** Initialize the volue parameters with sensible defaults.
 */
OutputVolumeParams::OutputVolumeParams(DoubleBoundingBox3D extent, double inputSpacing) :
	mInputSpacing(inputSpacing), mMaxVolumeSize(32*1000)
{
	mImage.setSpacingKeepDim(Eigen::Array3d(inputSpacing, inputSpacing, inputSpacing));
	mImage.setDimKeepBoundsAlignSpacing(extent.range());

	this->constrainVolumeSize();
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
