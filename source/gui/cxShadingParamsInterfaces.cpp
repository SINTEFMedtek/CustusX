/*
 *  cxShadingParamsInterfaces.cpp
 *  CustusX3
 *
 *  Created by Ole Vegard Solberg on 6/17/10.
 *  Copyright 2010 SINTEF. All rights reserved.
 *
 */
#include "cxShadingParamsInterfaces.h"
#include "cxDataManager.h"

namespace cx
{
DoubleDataAdapterShadingBase::DoubleDataAdapterShadingBase()
{
  connect(dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(activeImageChanged()));
  connect(dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SIGNAL(changed()));
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
  if (ssc::similar(val, mImage->getShadingAmbient()))
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
  if (ssc::similar(val, mImage->getShadingDiffuse()))
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
  if (ssc::similar(val, mImage->getShadingSpecular()))
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
  if (ssc::similar(val, mImage->getShadingSpecularPower()))
    return false;
  mImage->setShadingSpecularPower(val);
  return true;
}

  
}// namespace cx

