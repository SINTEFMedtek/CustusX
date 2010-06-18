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
DoubleDataInterfaceShadingBase::DoubleDataInterfaceShadingBase()
{
  connect(dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(activeImageChanged()));
  connect(dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SIGNAL(changed()));
}
void DoubleDataInterfaceShadingBase::activeImageChanged()
{  
  mImage = dataManager()->getActiveImage();
  emit changed();
}

double DoubleDataInterfaceShadingAmbient::getValue() const
{
  if (!mImage)
    return 0.0;
  return mImage->getShadingAmbient();
}

bool DoubleDataInterfaceShadingAmbient::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (ssc::similar(val, mImage->getShadingAmbient()))
    return false;
  mImage->setShadingAmbient(val);
  return true;
}


double DoubleDataInterfaceShadingDiffuse::getValue() const
{
  if (!mImage)
    return 0.0;
  return mImage->getShadingDiffuse();
}
bool DoubleDataInterfaceShadingDiffuse::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (ssc::similar(val, mImage->getShadingDiffuse()))
    return false;
  mImage->setShadingDiffuse(val);
  return true;
}


double DoubleDataInterfaceShadingSpecular::getValue() const 
{ 
  if (!mImage)
    return 0.0;
  return mImage->getShadingSpecular();
}
bool DoubleDataInterfaceShadingSpecular::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (ssc::similar(val, mImage->getShadingSpecular()))
    return false;
  mImage->setShadingSpecular(val);
  return true;
}


double DoubleDataInterfaceShadingSpecularPower::getValue() const 
{ 
  if (!mImage)
    return 0.0;
  return mImage->getShadingSpecularPower();
}
bool DoubleDataInterfaceShadingSpecularPower::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (ssc::similar(val, mImage->getShadingSpecularPower()))
    return false;
  mImage->setShadingSpecularPower(val);
  return true;
}

  
}// namespace cx

