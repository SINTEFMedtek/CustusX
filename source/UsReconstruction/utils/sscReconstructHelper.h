#ifndef SSCRECONSTRUCTHELPER_H_
#define SSCRECONSTRUCTHELPER_H_

#include <vector>
#include "sscTransform3D.h"
#include "sscReconstructAlgorithm.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
/**
 *  \class Planes
 *
 *\brief This struct contains three points that defines a plane and
 * the elements of the plane equation: n*r+d = 0
 *
 *  \author Ole Vegard Solberg
 *  \date 23/6/10
 */
struct Planes
{
  std::vector<ssc::Vector3D> mPoints;
  ssc::Vector3D mNormal;///< Elements of the plane equation n*r+d = 0
  double mDistance;///< Elements of the plane equation n*r+d = 0
};
  
  std::vector<Planes> generate_planes(std::vector<TimedPosition> frameInfo, 
                                      ImagePtr frameData);
  vtkImageDataPtr generateVtkImageData(Vector3D dim, Vector3D spacing, const unsigned char initValue); 

}//namespace ssc
#endif //SSCRECONSTRUCTHELPER_H_
