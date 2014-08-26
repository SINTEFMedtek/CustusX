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

#include "cxShadingParamsInterfaces.h"
#include "cxDataManager.h"
#include "cxImage.h"

namespace cx
{
DoubleDataAdapterShadingBase::DoubleDataAdapterShadingBase()
{
	mActiveImageProxy = ActiveImageProxy::New(dataService());
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChanged()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(changed()));

}
void DoubleDataAdapterShadingBase::activeImageChanged()
{  
  mImage = dataManager()->getActiveImage();
  emit changed();
}

double DoubleDataAdapterShadingAmbient::getValue() const
{
  if (!mImage)
    return 0.0;
  return mImage->getShadingAmbient();
}

bool DoubleDataAdapterShadingAmbient::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingAmbient()))
    return false;
  mImage->setShadingAmbient(val);
  return true;
}


double DoubleDataAdapterShadingDiffuse::getValue() const
{
  if (!mImage)
    return 0.0;
  return mImage->getShadingDiffuse();
}
bool DoubleDataAdapterShadingDiffuse::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingDiffuse()))
    return false;
  mImage->setShadingDiffuse(val);
  return true;
}


double DoubleDataAdapterShadingSpecular::getValue() const 
{ 
  if (!mImage)
    return 0.0;
  return mImage->getShadingSpecular();
}
bool DoubleDataAdapterShadingSpecular::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingSpecular()))
    return false;
  mImage->setShadingSpecular(val);
  return true;
}


double DoubleDataAdapterShadingSpecularPower::getValue() const 
{ 
  if (!mImage)
    return 0.0;
  return mImage->getShadingSpecularPower();
}
bool DoubleDataAdapterShadingSpecularPower::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingSpecularPower()))
    return false;
  mImage->setShadingSpecularPower(val);
  return true;
}

  
}// namespace cx

