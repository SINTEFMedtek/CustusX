/*
 * sscImageAlgorithms.h
 *
 *  Created on: Nov 11, 2010
 *      Author: christiana
 */

#ifndef SSCIMAGEALGORITHMS_H_
#define SSCIMAGEALGORITHMS_H_

#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"

namespace ssc
{

ImagePtr resampleImage(ImagePtr image, const Vector3D spacing, QString uid="", QString name="");
ImagePtr resampleImage(ImagePtr image, Transform3D refMi);
ImagePtr cropImage(ImagePtr image);

//vtkImageDataPtr CropAndClipImageTovtkImageData()


} // namespace ssc


#endif /* SSCIMAGEALGORITHMS_H_ */
