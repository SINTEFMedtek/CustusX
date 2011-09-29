/*
 *  sscThunderVNNReconstructAlgorithm.h
 *
 *  Created by Ole Vegard Solberg on 5/6/10.
 *
 */

#ifndef SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_
#define SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_

#include "sscReconstructAlgorithm.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"

namespace ssc
{
class ThunderVNNReconstructAlgorithm : public ReconstructAlgorithm
{
public:
  ThunderVNNReconstructAlgorithm(QString shaderPath);
  virtual ~ThunderVNNReconstructAlgorithm() {}
  virtual QString getName() const;
  virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
  virtual void reconstruct(std::vector<TimedPosition> frameInfo, 
                           //ImagePtr frameData,
                           USFrameDataPtr frameData,
                           ImagePtr outputData,
                           ImagePtr frameMask,
                           QDomElement settings);

	StringDataAdapterXmlPtr mProcessorOption;///< Select the processor to run the ocl code on (CPU/GPU)
private:
	DoubleDataAdapterXmlPtr mDistanceOption;
	BoolDataAdapterXmlPtr mPrintOpenCLInfoOption;
  QString mShaderPath;
};

}//namespace
#endif //SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_
