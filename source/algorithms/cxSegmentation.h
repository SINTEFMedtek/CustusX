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

namespace cx
{

class Segmentation
{
public:
  ssc::MeshPtr contour(ssc::ImagePtr image, QString outputBasePath, int threshold,
      double decimation=0.8, bool reduceResolution=true, bool smoothing=true);
  ssc::ImagePtr segment(ssc::ImagePtr image, QString outputBasePath, int threshold,
      bool useSmothing=true, double smoothSigma=0.5);
  ssc::ImagePtr centerline(ssc::ImagePtr image, QString outputBasePath);
  //void tubeContour(ssc::ImagePtr image, QString outputBasePath);
private:
};

} // namespace cx


#endif /* CXSEGMENTATION_H_ */
