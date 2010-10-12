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
  void contour(ssc::ImagePtr image, QString outputBasePath, int threshold);
  void segment(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmothing=true, double smoothSigma=0.5);
private:
};

} // namespace cx


#endif /* CXSEGMENTATION_H_ */
