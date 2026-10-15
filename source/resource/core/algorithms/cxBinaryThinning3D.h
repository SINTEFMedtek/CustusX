/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXBINARYTHINNING3D_H
#define CXBINARYTHINNING3D_H

#include "cxResourceExport.h"

#include <vector>
#include "vtkForwardDeclarations.h"

namespace cx
{

/** One voxel wide skeleton of a binary 3D volume.
 *
 * Parallel thinning with a 26-neighbourhood for the foreground and a 6-neighbourhood
 * for the background, as described in:
 *
 * T.C. Lee, R.L. Kashyap, and C.N. Chu.
 * Building skeleton models via 3-D medial surface/axis thinning algorithms.
 * Computer Vision, Graphics, and Image Processing, 56(6):462--478, 1994.
 *
 * Port of the ITK filter BinaryThinningImageFilter3D by Hanno Homann, giving the same result.
 *
 * \ingroup cx_resource_core_algorithms
 */
class cxResource_EXPORT BinaryThinning3D
{
public:
	/** Skeleton of the voxels equal to label, as a VTK_SHORT image with values 0/1.
	 * Values of the first component are converted to short before the comparison. The output extent starts at 0. */
	static vtkImageDataPtr skeleton(vtkImageDataPtr image, int label);
	/** Thin a volume in place. Non-zero voxels are foreground. Voxels are stored x fastest, then y, then z. */
	static void thin(std::vector<unsigned char>& voxels, int dimX, int dimY, int dimZ);
};

} // namespace cx

#endif // CXBINARYTHINNING3D_H
