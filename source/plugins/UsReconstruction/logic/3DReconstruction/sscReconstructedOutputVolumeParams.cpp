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
	mExtent(0, 0, 0, 0, 0, 0), mInputSpacing(0), m_rMd(Transform3D::Identity()),
	mDim(0, 0, 0), mSpacing(0), mMaxVolumeSize(32)
{

}
/** Initialize the volue parameters with sensible defaults.
 */
OutputVolumeParams::OutputVolumeParams(DoubleBoundingBox3D extent, double inputSpacing) :
	mExtent(extent), mInputSpacing(inputSpacing), mMaxVolumeSize(32)
{
	// Calculate optimal output image spacing and dimensions based on US frame spacing
	this->setSpacing(mInputSpacing);
	this->constrainVolumeSize();
}

unsigned long OutputVolumeParams::getVolumeSize() const
{
	return mDim[0] * mDim[1] * mDim[2];;
}

/** Set a spacing, recalculate dimensions.
 */
void OutputVolumeParams::setSpacing(double spacing)
{
	mSpacing = spacing;
	Vector3D v = mExtent.range() / mSpacing;
	mDim << ::ceil(v[0]), ::ceil(v[1]), ::ceil(v[2]);
}
double OutputVolumeParams::getSpacing() const
{
	return mSpacing;
}
/** Set one of the dimensions explicitly, recalculate other dims and spacing.
 */
void OutputVolumeParams::setDim(int index, int val)
{
	setSpacing(mExtent.range()[index] / val);
}
Eigen::Array3i OutputVolumeParams::getDim() const
{
	return mDim;
}
/** Increase spacing in order to keep size below a max size
 */
void OutputVolumeParams::constrainVolumeSize()
{
	this->setSpacing(mInputSpacing); // reset to default values

	// Reduce output volume size if optimal volume size is too large
	unsigned long volumeSize = getVolumeSize();
	unsigned long maxVolumeSize = this->getMaxVolumeSize();
	if (volumeSize > maxVolumeSize)
	{
		Vector3D ext = mExtent.range();
		double newSpacing = pow(ext[0]*ext[1]*ext[2] / double(maxVolumeSize), 1 / 3.0);
		this->setSpacing(newSpacing);
	}
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
	return mExtent;
}

double OutputVolumeParams::getInputSpacing()
{
	return mInputSpacing;
}

} // namespace cx
