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
  ThunderVNNReconstructAlgorithm(QString shaderPath);
  virtual ~ThunderVNNReconstructAlgorithm() {}
  virtual QString getName() const { return "ThunderVNN"; }
  virtual void getSettings(QDomElement root);
  virtual void reconstruct(std::vector<TimedPosition> frameInfo, 
                           ImagePtr frameData,
                           ImagePtr outputData,
                           ImagePtr frameMask,
                           QDomElement settings);
private:
  QString mShaderPath;
};

}//namespace
#endif //SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_
