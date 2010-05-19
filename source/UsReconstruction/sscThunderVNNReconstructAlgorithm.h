/*
 *  sscThunderVNNReconstructAlgorithm.h
 *
 *  Created by Ole Vegard Solberg on 5/6/10.
 *
 */

#ifndef SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_
#define SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_

#include "sscReconstructAlgorithm.h"


namespace ssc
{
class ThunderVNNReconstructAlgorithm : public ReconstructAlgorithm
{
public:
  ThunderVNNReconstructAlgorithm();
  virtual ~ThunderVNNReconstructAlgorithm() {}
  virtual void reconstruct(std::vector<TimedPosition> frameInfo, 
                           ImagePtr frameData,
                           ImagePtr outputData,
                           ImagePtr frameMask);
};

}//namespace
#endif //SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_
