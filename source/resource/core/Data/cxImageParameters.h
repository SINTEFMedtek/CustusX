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
 * \ingroup cx_resource_core_data
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
	QString mParentVolume;

	double getVolume();
	unsigned long getNumVoxels() const { return mDim.prod(); }

	void setDimKeepBoundsAlignSpacing(Eigen::Array3d bounds);
	void limitVoxelsKeepBounds(unsigned long maxVolumeSize);
	void setSpacingKeepDim(Eigen::Array3d spacing);
	void changeToUniformSpacing();

	Eigen::Array3i getDim() const;
	Eigen::Array3d getSpacing() const;
	Eigen::Array3d getBounds();

	void print(std::ostream& s, vtkIndent indent);

private:
	Eigen::Array3i mDim;
	Eigen::Array3d mSpacing;

	void alignSpacingKeepDim(Eigen::Array3d bounds);
	void setDimFromExtent(Eigen::Array3i extent);
};

}// namespace cx
#endif // CXIMAGEPARAMETERS_H

