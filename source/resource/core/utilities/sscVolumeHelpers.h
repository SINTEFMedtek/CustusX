#ifndef SSCVOLUMEHELPERS_H_
#define SSCVOLUMEHELPERS_H_

#include <map>
#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"

namespace cx
{
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Data> DataPtr;

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
 * Get information about a ssc volume.
 */
std::map<std::string, std::string> getDisplayFriendlyInfo(ImagePtr image);

int calculateNumVoxelsWithMaxValue(ImagePtr image);///<Find number of voxels containing max scalar value
int calculateNumVoxelsWithMinValue(ImagePtr image);///<Find number of voxels containing min scalar value

DoubleBoundingBox3D findEnclosingBoundingBox(std::vector<DataPtr> data, Transform3D qMr = Transform3D::Identity());
DoubleBoundingBox3D findEnclosingBoundingBox(std::vector<ImagePtr> data, Transform3D qMr = Transform3D::Identity());
/**
 * \}
 */

}


#endif /* SSCVOLUMEHELPERS_H_ */
