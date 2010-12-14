/*
 *  sscReconstructHelper.cpp
 *  Created by Ole Vegard Solberg on 23/6/10.
 */


#include "sscReconstructHelper.h"

#include <vtkImageData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include "sscTransform3D.h"
#include "sscImage.h"

namespace ssc 
{
  
std::vector<Planes> generate_planes(std::vector<TimedPosition> frameInfo, 
                                         ImagePtr frameData)
{
  vtkImageDataPtr input = frameData->getBaseVtkImageData();
  ssc::Vector3D dims(input->GetDimensions());
  ssc::Vector3D spacing(input->GetSpacing());
  
  
  ssc::Vector3D corner0(0.0f,0.0f,0.0f);
  ssc::Vector3D cornerx(dims[0]*spacing[0],0.0f,0.0f);
  ssc::Vector3D cornery(0.0f,dims[1]*spacing[1],0.0f);
  
  
  std::vector<Planes> retval;
  for (unsigned int n = 0; n < frameInfo.size(); n++)
  {
    Planes plane;
    plane.mPoints.resize(3);
    ssc::Vector3D a = frameInfo[n].mPos.coord(corner0);
    ssc::Vector3D b = frameInfo[n].mPos.coord(cornerx);
    ssc::Vector3D c = frameInfo[n].mPos.coord(cornery);
    
    ssc::Vector3D normal = cross(b-a, c-a).normal();
    double d = -dot(normal,a);
    
    plane.mPoints[0] = a;
    plane.mPoints[1] = b;
    plane.mPoints[2] = c;
    plane.mNormal = normal;
    plane.mDistance = d;
    
    retval.push_back(plane);
  }
  return retval;
}
  
}//namespace
