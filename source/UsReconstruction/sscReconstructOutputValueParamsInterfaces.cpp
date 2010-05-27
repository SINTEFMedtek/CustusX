/*
 * sscReconstructOutputValueParamsInterfaces.cpp
 *
 *  Created on: May 27, 2010
 *      Author: christiana
 */

#include "sscReconstructOutputValueParamsInterfaces.h"
#include "sscReconstructer.h"

namespace ssc
{

DoubleDataInterfaceOutputValueParams::DoubleDataInterfaceOutputValueParams(ReconstructerPtr reconstructer) :
    mReconstructer(reconstructer)
{
  connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SIGNAL(changed()));
}

double DoubleDataInterfaceOutputValueParams::getValue() const
{
  OutputVolumeParams par = mReconstructer->getOutputVolumeParams();
  return this->getValue(&par);
}

bool DoubleDataInterfaceOutputValueParams::setValue(double val)
{
  OutputVolumeParams par = mReconstructer->getOutputVolumeParams();
  if (similar(val, this->getValue(&par)))
    return false;
  //std::cout << "DoubleDataInterfaceOutputValueParams::setValue():" << this->getValueName() << std::endl;
  this->setValue(&par, val);
  mReconstructer->setOutputVolumeParams(par);
  return true;
}

} // namespace ssc
