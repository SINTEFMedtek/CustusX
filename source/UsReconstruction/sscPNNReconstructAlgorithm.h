/*
 *  sscPNNReconstructAlgorithm.h
 *
 *  Created by Ole Vegard Solberg on 23/6/10.
 *
 */

#ifndef SSCPNNRECONSTRUCTALGORITHM_H_
#define SSCPNNRECONSTRUCTALGORITHM_H_

#include <vtkImageData.h>
#include "sscReconstructAlgorithm.h"
#include "sscImage.h"


namespace ssc
{
class PNNReconstructAlgorithm : public ReconstructAlgorithm
{
public:
  PNNReconstructAlgorithm();
  virtual ~PNNReconstructAlgorithm() {}
  virtual QString getName() const { return "PNN"; }
  virtual void getSettings(QDomElement root);
  virtual void reconstruct(std::vector<TimedPosition> frameInfo, 
                           ImagePtr frameData,
                           ImagePtr outputData,
                           ImagePtr frameMask);
private:
  bool validPixel(int x, int y, int* dims, unsigned char* rawPointer)
  {
    return (x >= 0) && (x < dims[0])
    &&     (y >= 0) && (y < dims[1])
    && (rawPointer[x + y*dims[0]] != 0);
  }
  
  bool validVoxel(int x, int y, int z, int* dims)
  {
    return (x >= 0) && (x < dims[0])
    &&     (y >= 0) && (y < dims[1])
    &&     (z >= 0) && (z < dims[2]);
  }
  
  void interpolate(ImagePtr inputData, ImagePtr outputData);
};

}//namespace ssc
#endif //SSCPNNRECONSTRUCTALGORITHM_H_
