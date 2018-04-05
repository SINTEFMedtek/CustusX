/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
