/*
 * sscImageAlgorithms.h
 *
 *  \date Nov 11, 2010
 *      \author christiana
 */

#ifndef SSCIMAGEALGORITHMS_H_
#define SSCIMAGEALGORITHMS_H_

#include <map>
#include <vector>
#include <QDateTime>
#include <QRegExp>
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "vtkForwardDeclarations.h"
#include "sscBoundingBox3D.h"

namespace ssc
{

/**
* \file
*
* Various image algorithms. Mostly wrappers around ITK.
*
* \addtogroup cxResourceAlgorithms
*
*
* @{
*/

ImagePtr resampleImage(ImagePtr image, const Vector3D spacing, QString uid="", QString name="");
ImagePtr resampleImage(ImagePtr image, Transform3D refMi);
vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox);
ImagePtr cropImage(ImagePtr image);
ImagePtr duplicateImage(ImagePtr image);
vtkImageDataPtr convertImageToUnsigned(ImagePtr image);

/**
 */
QDateTime extractTimestamp(QString text);

/**Assume T is ssc::DataPtr or a derived class.
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

/**Assume T is ssc::DataPtr or a derived class.
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


//vtkImageDataPtr CropAndClipImageTovtkImageData()

/**
* @}
*/

} // namespace ssc



#endif /* SSCIMAGEALGORITHMS_H_ */
