/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxReconstructOutputValueParamsInterfaces.h"

#include "cxReconstructedOutputVolumeParams.h"
#include "cxUsReconstructionService.h"

namespace cx
{

DoublePropertyOutputValueParams::DoublePropertyOutputValueParams(UsReconstructionServicePtr reconstructer) :
    mReconstructer(reconstructer)
{
	connect(mReconstructer.get(), &UsReconstructionService::paramsChanged, this, &Property::changed);
}

double DoublePropertyOutputValueParams::getValue() const
{
  OutputVolumeParams par = mReconstructer->getOutputVolumeParams();
  return this->getValue(&par);
}

bool DoublePropertyOutputValueParams::setValue(double val)
{
  OutputVolumeParams par = mReconstructer->getOutputVolumeParams();
  if (similar(val, this->getValue(&par)))
    return false;
  this->setValue(&par, val);
  mReconstructer->setOutputVolumeParams(par);
  return true;
}

double DoublePropertySpacing::getValue(OutputVolumeParams *params) const { return params->getSpacing(); }

void DoublePropertySpacing::setValue(OutputVolumeParams *params, double val) { params->setSpacing(val); }

double DoublePropertyXDim::getValue(OutputVolumeParams *params) const { return params->getDim()[0]; }

void DoublePropertyXDim::setValue(OutputVolumeParams *params, double val) { params->setDim(0, val); }

double DoublePropertyYDim::getValue(OutputVolumeParams *params) const { return params->getDim()[1]; }

void DoublePropertyYDim::setValue(OutputVolumeParams *params, double val) { params->setDim(1, val); }

double DoublePropertyZDim::getValue(OutputVolumeParams *params) const { return params->getDim()[2]; }

void DoublePropertyZDim::setValue(OutputVolumeParams *params, double val) { params->setDim(2, val); }

} // namespace cx
