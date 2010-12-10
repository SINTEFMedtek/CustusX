#ifndef CXTRACKINGDATATOVOLUME_H_
#define CXTRACKINGDATATOVOLUME_H_

#include "sscImage.h"
#include "sscTool.h"
#include "sscVector3D.h"

namespace cx
{
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

  void setInput(ssc::TimedTransformMap map_prMt); ///< the map should contain transforms to tools point [0,0,0] (the tip of the tool)
  ssc::ImagePtr getOutput(); ///< the generated image in image (d) space

private:
  ssc::DoubleBoundingBox3D getBoundingBox(ssc::TimedTransformMap& map_prMt);///< Create bounding box around input positions
  ssc::ImagePtr createEmptyImage(ssc::DoubleBoundingBox3D extent_pr, int spacing);///< Create an empty sscImage. Creates an unsigned char volume.
  std::vector<ssc::Vector3D> extractPoints(ssc::TimedTransformMap& map_prMt);///< Extracts the tool point (0,0,0) (aka: the tooltip) in pr space
  void insertPoints(ssc::ImagePtr image_d, std::vector<ssc::Vector3D> points_pr);///< Insert points as dots(value 1) in a volume.

  ssc::ImagePtr mImage;
  int mSpacing; ///< the spacing used to create the image
};
}//cx

#endif /* CXTRACKINGDATATOVOLUME_H_ */
