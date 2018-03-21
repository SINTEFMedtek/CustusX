/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIMAGEPARAMETERS_H
#define CXIMAGEPARAMETERS_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxVector3D.h"
#include "cxTransform3D.h"

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
class cxResource_EXPORT ImageParameters
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

