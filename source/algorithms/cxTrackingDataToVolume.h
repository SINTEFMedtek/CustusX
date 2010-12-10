/*
 * cxTrackingDataToVolume.h
 *
 *  Created on: Dec 9, 2010
 *      Author: olevs
 */
#ifndef CXTRACKINGDATATOVOLUME_H_
#define CXTRACKINGDATATOVOLUME_H_

#include "sscImage.h"
#include "sscTool.h"
#include "sscVector3D.h"

namespace cx
{
class TrackingDataToVolume
{
public:
  TrackingDataToVolume();
  ~TrackingDataToVolume();

  void setInput(ssc::TimedTransformMap map_prMt);
  ssc::ImagePtr getOutput();

private:
  ssc::ImagePtr mImage;
  int mSpacing;
  ssc::DoubleBoundingBox3D getBoundingBox(ssc::TimedTransformMap& map_prMt);///< Create bounding box around input positions
  ssc::ImagePtr createEmptyImage(ssc::DoubleBoundingBox3D extent_pr, int spacing);///< Create an empty sscImage. Creates an unsigned char volume
  std::vector<ssc::Vector3D> extractPoints(ssc::TimedTransformMap& map_prMt);///< Convert from transforms to points
  void insertPoints(ssc::ImagePtr image_pr, std::vector<ssc::Vector3D> points_pr);///< Insert points as dots in a volume.
};
}//cx

#endif /* CXTRACKINGDATATOVOLUME_H_ */
