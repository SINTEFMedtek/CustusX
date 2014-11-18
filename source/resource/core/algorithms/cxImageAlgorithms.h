/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
class DataManager;

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
