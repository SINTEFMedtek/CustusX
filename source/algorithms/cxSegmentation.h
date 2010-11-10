/*
 * cxSegmentation.h
 *
 *  Created on: Oct 12, 2010
 *      Author: christiana
 */

#ifndef CXSEGMENTATION_H_
#define CXSEGMENTATION_H_

#include <QString>
#include "sscForwardDeclarations.h"

#include "sscTransform3D.h"

//namespace ssc
//{
//  ImagePtr cropImage(ImagePtr image, Transform3D qMd, DoubleBoundingBox3D bb_q, Vector3D outputSpacing);
//}

namespace cx
{

class Segmentation
{
public:
  ssc::MeshPtr contour(ssc::ImagePtr image, QString outputBasePath, int threshold,
      double decimation=0.8, bool reduceResolution=false, bool smoothing=true);
  ssc::ImagePtr segment(ssc::ImagePtr image, QString outputBasePath, int threshold,
      bool useSmothing=true, double smoothSigma=0.5);
  ssc::ImagePtr centerline(ssc::ImagePtr image, QString outputBasePath);
  ssc::ImagePtr resample(ssc::ImagePtr image, ssc::ImagePtr reference, QString outputBasePath, double margin);

private:
};

} // namespace cx


#endif /* CXSEGMENTATION_H_ */
