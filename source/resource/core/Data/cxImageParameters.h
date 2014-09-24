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

