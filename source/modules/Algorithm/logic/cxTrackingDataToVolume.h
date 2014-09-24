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

#ifndef CXTRACKINGDATATOVOLUME_H_
#define CXTRACKINGDATATOVOLUME_H_

#include "cxPluginAlgorithmExport.h"

#include "cxImage.h"
#include "cxTool.h"
#include "cxVector3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_algorithm
 * @{
 */

/**
 * TrackingDataToVolume
 *
 * \brief Algorithm that converts tool points, in pr space, into a binary unsigned char (8bits) image.
 *
 * \warning Algorithm creates only images with the same spacing in all directions. Default is 0,2.
 *
 * \date Dec 9, 2010
 *
 * \author Ole Vegard Solberg, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxPluginAlgorithm_EXPORT TrackingDataToVolume
{
public:
  TrackingDataToVolume();
  ~TrackingDataToVolume();

  void setInput(TimedTransformMap map_prMt, int padding=15); ///< the map should contain transforms to tools point [0,0,0] (the tip of the tool)
  ImagePtr getOutput(); ///< the generated image in image (d) space

private:
  inline bool rangeCheck(int x, int y, int z, int* extent)
  {
    if (x < extent[0] ||
        x > extent[1] ||
        y < extent[2] ||
        y > extent[3] ||
        z < extent[4] ||
        z > extent[5])
      return false;
    else
      return true;
  } ///< Checks that the given x,y,z point is inside the given extent

  DoubleBoundingBox3D getBoundingBox(TimedTransformMap& map_prMt);///< Create bounding box around input positions
  ImagePtr createEmptyImage(DoubleBoundingBox3D bounds_pr, double spacing);///< Create an empty sscImage. Creates an unsigned char volume.
  std::vector<Vector3D> extractPoints(TimedTransformMap& map_prMt);///< Extracts the tool point (0,0,0) (aka: the tooltip) in pr space
  void insertPoints(ImagePtr image_d, std::vector<Vector3D> points_pr, int padding_voxels);///< Insert points as dots(value 1) in a volume.
  void writeVoxelValuesWithPadding(Vector3D point_voxel, vtkImageDataPtr data_pr, int padding_voxels); ///< Writes values into the voxels and appends a padding around

  ImagePtr mImage;
};

/**
 * @}
 */
}//cx

#endif /* CXTRACKINGDATATOVOLUME_H_ */
