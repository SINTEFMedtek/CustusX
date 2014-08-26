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
#include "cxImageParameters.h"
#include "cxReporter.h"
#include "cxTypeConversions.h"

namespace cx
{
ImageParameters::ImageParameters() :
	mDim(Eigen::Array3i(0,0,0)),
	mSpacing(cx::Vector3D(1,1,1)),
	mParentVolume(""),
	m_rMd(cx::Transform3D::Identity())
{
}

ImageParameters::ImageParameters(Eigen::Array3i dim, cx::Vector3D spacing, QString parent, cx::Transform3D rMd) :
	mDim(dim),
	mSpacing(spacing),
	mParentVolume(parent),
	m_rMd(rMd)
{
}

Eigen::Array3i ImageParameters::getDim() const
{
	return mDim;
}

Eigen::Array3d ImageParameters::getSpacing() const
{
	return mSpacing;
}

Eigen::Array3d ImageParameters::getBounds()
{
	return (mDim - 1).cast<double>() * mSpacing;
}

double ImageParameters::getVolume()
{
	return this->getBounds().prod();
}

void ImageParameters::setDimKeepBoundsAlignSpacing(Eigen::Array3d bounds)
{
	Eigen::Array3d dim = bounds / mSpacing;
	mDim = round(dim).cast<int>();
	mDim += 1;
	this->alignSpacingKeepDim(bounds);
}

void ImageParameters::alignSpacingKeepDim(Eigen::Array3d bounds)
{
	for (unsigned i = 0; i < 3; ++i)
	{
		//Set spacing to 1 if one of the axes is degenerated
		if((mDim[i] == 1) && similar(bounds[i], 0.0))
			mSpacing[i] = 1;
		else
			mSpacing[i] = bounds[i] / double(mDim[i]-1);
	}
}

void ImageParameters::setSpacingKeepDim(Eigen::Array3d spacing)
{
	mSpacing = spacing;
}

void ImageParameters::setDimFromExtent(Eigen::Array3i extent)
{
	mDim = extent + 1;
}

/** Reduce the voxel size of the volume below maxVoxels.
	*  - keep bounds constant
	*  - keep ratio between spacing components
	*/
void ImageParameters::limitVoxelsKeepBounds(unsigned long maxVoxels)
{
	if (this->getNumVoxels() <= maxVoxels)
		return;

	if(mDim.minCoeff() == 1) //At least one of the dimensions == 1
	{
			reportError("ImageParameters::limitVoxelsKeepBounds() only work with 3D images");
			return;
	}
	// i: input spacing
	// s: (output) spacing
	// b: bounds
	// e: extent
	// d: dim
	//
	// Relations:
	//    e = d-1
	//    b = s*d
	//
	// Ve: The volume of the extent, i.e. volume in voxel space
	//     Ve = e0*e1*e2
	//     Ve = pow(pow(voxelCount,1/3)-1, 3) //convert from dim to extent
	// Vb: The volume of the bounds, ie in physical space
	//     Vb = b0*b1*b2
	//
	// Start with the Extent volume:
	//     Ve = e0*e1*e2 = b0/s0 * b1/s1 * b2/s2 = Vb / (s0*s1*s2)
	//
	// Keep the ratios between the spacing components from the input spacing:
	//     f1 = i1/i0 = s1/s0
	//     f2 = i2/i0 = s2/s0
	// This gives:
	//     s1 = f1*s0
	//     s2 = f2*s0
	//
	// Plug into extent volume:
	//     Ve = Vb / (s0^3*f1*f2)
	//     s0^3 = Vb/(Ve*f1*f2)
	//     s0 = pow(Vb/(Ve*f1*f2), 1/3)

	// init
	Eigen::Array3d b = this->getBounds();
	Eigen::Array3d i = this->getSpacing();
	double f1 = i[1] / i[0];
	double f2 = i[2] / i[0];
	double Vb = b.prod();
	double Ve = pow(pow(double(maxVoxels),1.0/3)-1, 3);

	// generate spacing array
	Eigen::Array3d s;
	s[0] = pow(Vb/(Ve*f1*f2), 1.0/3);
	s[1] = f1 * s[0];
	s[2] = f2 * s[0];

	// set values
	mSpacing = s;
	Eigen::Array3i e = (b/s).cast<int>();
	// extents of 1 gives unstable results. extent 0 is an error.
//	e = e.max(Eigen::Array3i(1,1,1));
	this->setDimFromExtent(e);
	this->alignSpacingKeepDim(b); // change spacing to keep bounds
}

/** Change spacing to uniform
	*  - keep bounds constant
	*  - keep total voxel count constant
	*/
void ImageParameters::changeToUniformSpacing()
{
	Eigen::Array3d b = this->getBounds();
	double voxelCount = this->getNumVoxels();
	double Vb = b.prod();
	double Ve = pow(pow(double(voxelCount),1.0/3)-1, 3);
	double spacing = pow(Vb/Ve, 1.0/3);
	Eigen::Array3d s(spacing,spacing,spacing);

	// set values
	mSpacing = Eigen::Array3d(spacing,spacing,spacing);
	Eigen::Array3i e = (b/s).cast<int>();
	this->setDimFromExtent(e);
}

void ImageParameters::print(std::ostream& s, vtkIndent indent)
{
	s << indent << "Dim: " << mDim << std::endl;
	s << indent << "Spacing: " << mSpacing << std::endl;
	s << indent << "Bounds: " << this->getBounds() << std::endl;
	s << indent << "NumVoxels: " << this->getNumVoxels() << std::endl;
	s << indent << "Parent Volume: " << mParentVolume << std::endl;
	s << indent << "rMd:\n" << m_rMd << std::endl;
}


}// namespace cx
