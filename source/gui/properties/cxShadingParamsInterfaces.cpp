/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxShadingParamsInterfaces.h"
#include "cxImage.h"
#include "cxActiveData.h"

namespace cx
{
DoublePropertyShadingBase::DoublePropertyShadingBase(ActiveDataPtr activeData) :
	mActiveData(activeData)
{
	mActiveImageProxy = ActiveImageProxy::New(mActiveData);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &DoublePropertyShadingBase::activeImageChanged);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &Property::changed);
	this->activeImageChanged();
}

void DoublePropertyShadingBase::activeImageChanged()
{  
  mImage = mActiveData->getActive<Image>();
  emit changed();
}

DoublePropertyShadingAmbient::DoublePropertyShadingAmbient(ActiveDataPtr activeData) :
	DoublePropertyShadingBase(activeData)
{
}

double DoublePropertyShadingAmbient::getValue() const
{
  if (!mImage)
    return 0.0;
  return mImage->getShadingAmbient();
}

bool DoublePropertyShadingAmbient::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingAmbient()))
    return false;
  mImage->setShadingAmbient(val);
  return true;
}


DoublePropertyShadingDiffuse::DoublePropertyShadingDiffuse(ActiveDataPtr activeData) :
	DoublePropertyShadingBase(activeData)
{

}

double DoublePropertyShadingDiffuse::getValue() const
{
  if (!mImage)
    return 0.0;
  return mImage->getShadingDiffuse();
}
bool DoublePropertyShadingDiffuse::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingDiffuse()))
    return false;
  mImage->setShadingDiffuse(val);
  return true;
}


DoublePropertyShadingSpecular::DoublePropertyShadingSpecular(ActiveDataPtr activeData) :
	DoublePropertyShadingBase(activeData)
{
}

double DoublePropertyShadingSpecular::getValue() const
{ 
  if (!mImage)
    return 0.0;
  return mImage->getShadingSpecular();
}
bool DoublePropertyShadingSpecular::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingSpecular()))
    return false;
  mImage->setShadingSpecular(val);
  return true;
}


DoublePropertyShadingSpecularPower::DoublePropertyShadingSpecularPower(ActiveDataPtr activeData) :
	DoublePropertyShadingBase(activeData)
{
}

double DoublePropertyShadingSpecularPower::getValue() const
{ 
  if (!mImage)
    return 0.0;
  return mImage->getShadingSpecularPower();
}
bool DoublePropertyShadingSpecularPower::setValue(double val)
{ 
  if (!mImage)
    return false;
  if (similar(val, mImage->getShadingSpecularPower()))
    return false;
  mImage->setShadingSpecularPower(val);
  return true;
}

  
}// namespace cx

