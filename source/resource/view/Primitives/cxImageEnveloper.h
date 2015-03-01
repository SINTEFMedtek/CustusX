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

#ifndef CXMULTIVOLUMEBOUNDINGBOX_H
#define CXMULTIVOLUMEBOUNDINGBOX_H

#include "cxResourceVisualizationExport.h"

#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include <vector>
#include "cxImageParameters.h"

namespace cx
{

typedef boost::shared_ptr<class ImageEnveloper> ImageEnveloperPtr;

/**
 * Generate a total bounding volume from a set of volumes.
 *
 * \ingroup cx_resource_view
 * \date 3 Oct 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT ImageEnveloper
{
public:
	static ImageEnveloperPtr create();
	ImageEnveloper();
	virtual ~ImageEnveloper() {}

	virtual void setImages(std::vector<ImagePtr> images);
	virtual ImagePtr getEnvelopingImage();
	void setMaxEnvelopeVoxels(long maxVoxels);

private:
	std::vector<ImagePtr> mImages;
	long mMaxEnvelopeVoxels;

	ImageParameters reduceToNumberOfVoxels(ImageParameters box, long maxVoxels);
	ImageParameters createEnvelopeParametersFromImage(ImagePtr img);
	ImageParameters selectParametersWithSmallestExtent(ImageParameters a, ImageParameters b);
	ImageParameters selectParametersWithFewestVoxels(ImageParameters a, ImageParameters b);
	ImagePtr createEnvelopeFromParameters(ImageParameters box);
	Eigen::Array3d getMinimumSpacingFromAllImages(Transform3D qMr);
	Eigen::Array3d getTransformedSpacing(Eigen::Array3d spacing, Transform3D qMd);
	int getMaxScalarRange();
};

} // namespace cx

#endif // CXMULTIVOLUMEBOUNDINGBOX_H
