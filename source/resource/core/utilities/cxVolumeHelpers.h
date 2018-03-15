/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVOLUMEHELPERS_H_
#define CXVOLUMEHELPERS_H_

#include "cxResourceExport.h"

#include <map>
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Data> DataPtr;

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

cxResource_EXPORT vtkImageDataPtr generateVtkImageData(Eigen::Array3i dim, Vector3D spacing, const unsigned char initValue, int components = 1);
cxResource_EXPORT vtkImageDataPtr generateVtkImageDataUnsignedShort(Eigen::Array3i dim,
									 Vector3D spacing,
									 const unsigned short initValue,
									 int components = 1);
cxResource_EXPORT vtkImageDataPtr generateVtkImageDataSignedShort(Eigen::Array3i dim,
									 Vector3D spacing,
									 const short initValue,
									 int components = 1);
cxResource_EXPORT vtkImageDataPtr generateVtkImageDataDouble(Eigen::Array3i dim, Vector3D spacing, double initValue);
cxResource_EXPORT void fillShortImageDataWithGradient(vtkImageDataPtr data, int maxValue);
cxResource_EXPORT ImagePtr createDerivedImage(PatientModelServicePtr dataManager, QString uid, QString name, vtkImageDataPtr raw, ImagePtr parent);


/**
  * Convert an Image to its unsigned version.
  *
  * The lowest value in image is found and the entire index range is shifted by this
  * amount. Exception: CT images are always shifted by 1024.
  *
  * The suggestedConvertedVolume is a pure optimization: Is set it will be used as the converted
  * output instead of doing the conversion once more. Can be used when only the LUT should be updated.
  */
cxResource_EXPORT ImagePtr convertImageToUnsigned(PatientModelServicePtr dataManager, ImagePtr image, vtkImageDataPtr suggestedConvertedVolume = vtkImageDataPtr(), bool verbose = true);

cxResource_EXPORT std::map<std::string, std::string> getDisplayFriendlyInfo(ImagePtr image);
cxResource_EXPORT std::map<std::string, std::string> getDisplayFriendlyInfo(vtkImageDataPtr image);
cxResource_EXPORT void printDisplayFriendlyInfo(std::map<std::string, std::string> map);

cxResource_EXPORT int calculateNumVoxelsWithMaxValue(ImagePtr image);///<Find number of voxels containing max scalar value
cxResource_EXPORT int calculateNumVoxelsWithMinValue(ImagePtr image);///<Find number of voxels containing min scalar value

cxResource_EXPORT DoubleBoundingBox3D findEnclosingBoundingBox(std::vector<DataPtr> data, Transform3D qMr = Transform3D::Identity());
cxResource_EXPORT DoubleBoundingBox3D findEnclosingBoundingBox(std::vector<ImagePtr> data, Transform3D qMr = Transform3D::Identity());


cxResource_EXPORT vtkImageDataPtr convertImageDataToGrayScale(vtkImageDataPtr image);
cxResource_EXPORT vtkImageDataPtr convertFrom4To3Components(vtkImageDataPtr image);

cxResource_EXPORT vtkImageDataPtr convertImageDataTo8Bit(vtkImageDataPtr image, double windowWidth, double windowLevel);///< Have never been used or tested. Create a test for it

/** Call after the image contents has been changed throug GetScalarPointer()
  *
  * Set modified on a vtkImageData and inner classes.
  * This ensures that GetScalarRange() returns the correct
  * value.
  */
cxResource_EXPORT void setDeepModified(vtkImageDataPtr image);


/**
 * \}
 */

}


#endif /* CXVOLUMEHELPERS_H_ */
