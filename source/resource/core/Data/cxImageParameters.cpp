// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxImageParameters.h"

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

void ImageParameters::setDimFromBounds(Eigen::Array3d bounds)
{
	Eigen::Array3d dim = bounds / mSpacing;
	dim += 1;
	mDim = ceil(dim).cast<int>();
//	return ceil(dim).cast<int>();
}

}// namespace cx
