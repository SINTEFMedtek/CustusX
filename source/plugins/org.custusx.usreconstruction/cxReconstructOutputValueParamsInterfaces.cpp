/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxReconstructOutputValueParamsInterfaces.h"

#include "cxReconstructedOutputVolumeParams.h"
#include "cxUsReconstructionService.h"

namespace cx
{

DoubleDataAdapterOutputValueParams::DoubleDataAdapterOutputValueParams(UsReconstructionServicePtr reconstructer) :
    mReconstructer(reconstructer)
{
  connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SIGNAL(changed()));
}

double DoubleDataAdapterOutputValueParams::getValue() const
{
  OutputVolumeParams par = mReconstructer->getOutputVolumeParams();
  return this->getValue(&par);
}

bool DoubleDataAdapterOutputValueParams::setValue(double val)
{
  OutputVolumeParams par = mReconstructer->getOutputVolumeParams();
  if (similar(val, this->getValue(&par)))
    return false;
  //std::cout << "DoubleDataAdapterOutputValueParams::setValue():" << this->getDisplayName() << std::endl;
  this->setValue(&par, val);
  mReconstructer->setOutputVolumeParams(par);
  return true;
}

double DoubleDataAdapterSpacing::getValue(OutputVolumeParams *params) const { return params->getSpacing(); }

void DoubleDataAdapterSpacing::setValue(OutputVolumeParams *params, double val) { params->setSpacing(val); }

double DoubleDataAdapterXDim::getValue(OutputVolumeParams *params) const { return params->getDim()[0]; }

void DoubleDataAdapterXDim::setValue(OutputVolumeParams *params, double val) { params->setDim(0, val); }

double DoubleDataAdapterYDim::getValue(OutputVolumeParams *params) const { return params->getDim()[1]; }

void DoubleDataAdapterYDim::setValue(OutputVolumeParams *params, double val) { params->setDim(1, val); }

double DoubleDataAdapterZDim::getValue(OutputVolumeParams *params) const { return params->getDim()[2]; }

void DoubleDataAdapterZDim::setValue(OutputVolumeParams *params, double val) { params->setDim(2, val); }

} // namespace cx
