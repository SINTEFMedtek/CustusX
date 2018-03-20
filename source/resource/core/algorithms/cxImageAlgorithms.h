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
* Various image algorithms. Mostly wrappers around ITK.
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
