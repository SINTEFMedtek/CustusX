#ifndef CXTRACKINGDATATOVOLUME_H_
#define CXTRACKINGDATATOVOLUME_H_

#include "sscImage.h"
#include "sscTool.h"
#include "sscVector3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_algorithm
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
class TrackingDataToVolume
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
