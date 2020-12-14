/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMAGEALGORITHMS_H_
#define CXIMAGEALGORITHMS_H_

#include "cxResourceExport.h"

#include <map>
#include <vector>
#include <QDateTime>
#include <QRegExp>
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxTypeConversions.h"
#include "vtkForwardDeclarations.h"
#include "cxBoundingBox3D.h"

namespace cx
{

/**
* \file
*
* Various image algorithms. Mostly wrappers around ITK, and conversion from vtkImageData to QImage.
*
* \addtogroup cx_resource_core_algorithms
*
*
* @{
*/

cxResource_EXPORT ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, const Vector3D spacing, QString uid="", QString name="");
cxResource_EXPORT ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, Transform3D refMi);
cxResource_EXPORT vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox);
cxResource_EXPORT ImagePtr cropImage(PatientModelServicePtr dataManager, ImagePtr image);
cxResource_EXPORT ImagePtr duplicateImage(PatientModelServicePtr dataManager, ImagePtr image);

cxResource_EXPORT QImage vtkImageDataToQImage(vtkImageDataPtr imageData, bool overlay = false, QColor overlayColor = QColor(255, 255, 0, 70));
cxResource_EXPORT QRgb convertToQColor(unsigned char *colorsPtr, bool overlay, QColor overlayColor);
cxResource_EXPORT bool isDark(unsigned char *colorsPtr);
cxResource_EXPORT QRgb modifyOverlayColor(unsigned char *colorsPtr, QColor overlayColor);

/**
 * @brief createSlice Creates a 2D slice through a 3D volume. Result slice will be oriented for radiological view.
 * @param image Input 3D volume
 * @param planeType Output 2D slice plane type
 * @param outputSpacing Output slice image spacing
 * @param outputDimensions Output slice image dimanesions
 * @param sliceTool Slicing is performed at this tools position
 * @param patientModel CustusX Patient Model. Needed by slicing code
 * @param applyLUT Apply color 3D lookup table values to 2D output slice
 * @return 2D slice as vtkImageData
 */
cxResource_EXPORT vtkImageDataPtr createSlice(ImagePtr image, PLANE_TYPE planeType, Vector3D outputSpacing, Eigen::Array3i outputDimensions, ToolPtr sliceTool, PatientModelServicePtr patientModel, bool applyLUT);
cxResource_EXPORT vtkImageDataPtr createSlice(ImagePtr image, PLANE_TYPE planeType, Vector3D position, Vector3D target, double offset = 0, bool applyLUT = true);
cxResource_EXPORT std::vector<int> getSliceVoxelFrom3Dposition(ImagePtr image, PLANE_TYPE planeType, Vector3D position);
cxResource_EXPORT int getSliceNumberFrom3Dposition(ImagePtr image, PLANE_TYPE planeType, Vector3D position);
cxResource_EXPORT Vector3D get3DpositionFromSliceVoxel(ImagePtr image, PLANE_TYPE planeType, std::vector<int> voxel, int sliceNumber);
/**
 */
cxResource_EXPORT QDateTime extractTimestamp(QString text);

/**Assume T is DataPtr or a derived class.
 *
 * Return a vector of data sorted on acquisition time.
 */
template<class T>
std::vector<T> sortOnAcquisitionTime(std::map<QString, T> input)
{
  typedef std::multimap<QDateTime,T> SortedMap;
  SortedMap sorted;

  for (typename std::map<QString, T>::iterator iter=input.begin(); iter!=input.end(); ++iter)
  {
    sorted.insert(std::make_pair(iter->second->getAcquisitionTime(), iter->second));
  }

  std::vector<T> retval;
  for (typename SortedMap::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
    retval.push_back(iter->second);
  return retval;
}

/**Assume T is DataPtr or a derived class.
 *
 * Return a vector of data sorted on acquisition time.
 */
template<class T>
std::vector<T> sortOnGroupsAndAcquisitionTime(std::map<QString, T> input)
{
  typedef std::map<QString, T> DataMap;
  std::map<QDateTime, DataMap> bin;

  for (typename DataMap::iterator iter=input.begin(); iter!=input.end(); ++iter)
  {
    QDateTime binTS = extractTimestamp(iter->second->getUid());
    bin[binTS].insert(*iter);
  }


  std::vector<T> retval;

  for (typename std::map<QDateTime, DataMap>::iterator iter=bin.begin(); iter!=bin.end(); ++iter)
  {
    std::vector<T> localSorted = sortOnAcquisitionTime(iter->second);
    std::copy(localSorted.begin(), localSorted.end(), back_inserter(retval));
  }

  return retval;
}

/**
* @}
*/

} // namespace cx



#endif /* CXIMAGEALGORITHMS_H_ */
