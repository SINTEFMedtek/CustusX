/*
 * sscImageAlgorithms.h
 *
 *  Created on: Nov 11, 2010
 *      Author: christiana
 */

#ifndef SSCIMAGEALGORITHMS_H_
#define SSCIMAGEALGORITHMS_H_

#include <map>
#include <vector>
#include <QDateTime>
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"

namespace ssc
{

ImagePtr resampleImage(ImagePtr image, const Vector3D spacing, QString uid="", QString name="");
ImagePtr resampleImage(ImagePtr image, Transform3D refMi);
ImagePtr cropImage(ImagePtr image);


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

//vtkImageDataPtr CropAndClipImageTovtkImageData()


} // namespace ssc


#endif /* SSCIMAGEALGORITHMS_H_ */
