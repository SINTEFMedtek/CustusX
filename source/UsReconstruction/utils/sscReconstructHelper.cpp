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

vtkImageDataPtr generateVtkImageData(Vector3D dim, 
                                     Vector3D spacing,
                                     const unsigned char initValue)
{
  vtkImageDataPtr data = vtkImageDataPtr::New();
  data->SetSpacing(spacing[0], spacing[1], spacing[2]);
  data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
  data->SetScalarTypeToUnsignedChar();
  data->SetNumberOfScalarComponents(1);
  
  int scalarSize = dim[0]*dim[1]*dim[2];
  
  unsigned char *rawchars = (unsigned char*)malloc(scalarSize+1);
  std::fill(rawchars,rawchars+scalarSize, initValue);
  
  vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
  array->SetNumberOfComponents(1);
  //TODO: Whithout the +1 the volume is black 
  array->SetArray(rawchars, scalarSize+1, 0); // take ownership
  data->GetPointData()->SetScalars(array);
  
  rawchars[0] = 255;// A trick to get a full LUT in ssc::Image (automatic LUT generation)
  
  /*data->AllocateScalars();
   unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
   unsigned long N = data->GetNumberOfPoints();
   N--;//Don't understand this
   for (unsigned long i = 0; i < N; i++)
   dataPtr[i] = 100;
   
   //dataPtr[N] = 255;//This creates a black volume
   
   dataPtr[0] = 255;
   
   std::cout << "Reconstructer::generateOutputVolume() data->GetNumberOfPoints(): " 
   << N << std::endl;*/
  
  
  return data;
}
  
}//namespace
