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
#ifndef CXIMAGEPARAMETERS_H
#define CXIMAGEPARAMETERS_H

#include "sscVector3D.h"
#include "sscTransform3D.h"

namespace cx
{
/**
 * Information needed to create a cx::Image
 *
 * \date 8 Oct 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class ImageParameters
{
public:
	ImageParameters();
	ImageParameters(Eigen::Array3i dim, cx::Vector3D spacing, QString parent, cx::Transform3D rMd);

	Transform3D m_rMd;
	Eigen::Array3i mDim;
	Eigen::Array3d mSpacing;
	QString mParentVolume;
	double getVolume()
	{
		Eigen::Array3d extent = (mDim.cast<double>()-1)* mSpacing;
		return extent.prod();
	}
	long getNumVoxels() { return mDim.prod(); }

//	static Eigen::Array3i getDimFromExtent(Eigen::Array3d extent, Eigen::Array3d spacing);

	void setDimFromBounds(Eigen::Array3d bounds);
};

}// namespace cx
#endif // CXIMAGEPARAMETERS_H

