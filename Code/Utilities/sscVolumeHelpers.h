/*
 * sscVolumeHelpers.h
 *
 *  Created on: Dec 13, 2010
 *      Author: dev
 */

#ifndef SSCVOLUMEHELPERS_H_
#define SSCVOLUMEHELPERS_H_

#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
typedef boost::shared_ptr<class Image> ImagePtr;

/**
 * \addtogroup sscUtility
 * \{
 */

vtkImageDataPtr generateVtkImageData(Eigen::Array3i dim, Vector3D spacing, const unsigned char initValue, int components = 1);
vtkImageDataPtr generateVtkImageDataDouble(Eigen::Array3i dim, Vector3D spacing, double initValue);

/**
  * Convert an Image to its unsigned version.
  *
  * The lowest value in image is found and the entire index range is shifted by this
  * amount. Exception: CT images are always shifted by 1024.
  *
  * The suggestedConvertedVolume is a pure optimization: Is set it will be used as the converted
  * output instead of doing the conversion once more. Can be used when only the LUT should be updated.
  */
ImagePtr convertImageToUnsigned(ImagePtr image, vtkImageDataPtr suggestedConvertedVolume = vtkImageDataPtr(), bool verbose = true);

/**
 * \}
 */

}


#endif /* SSCVOLUMEHELPERS_H_ */
