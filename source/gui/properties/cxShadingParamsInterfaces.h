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

#ifndef CXSHADINGPARAMSINTERFACES_H_
#define CXSHADINGPARAMSINTERFACES_H_

#include "cxGuiExport.h"

#include "cxDoubleRange.h"
#include "cxDoubleWidgets.h"
#include "cxForwardDeclarations.h"
#include "cxActiveImageProxy.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Superclass for all shading parameters
 */
class cxGui_EXPORT DoublePropertyShadingBase : public DoublePropertyBase
{
  Q_OBJECT 
public:
  DoublePropertyShadingBase(ActiveDataPtr activeData);
  virtual ~DoublePropertyShadingBase() {}
  //virtual double getValue() const;
  //virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}  
private slots:
  void activeImageChanged();
protected:
  ImagePtr mImage;
  ActiveImageProxyPtr mActiveImageProxy;
  ActiveDataPtr mActiveData;
};
  
/** Interface for setting the ambient parameter for the shading.
 */
class cxGui_EXPORT DoublePropertyShadingAmbient : public DoublePropertyShadingBase
{
  Q_OBJECT
public:
  DoublePropertyShadingAmbient(ActiveDataPtr activeData);
  virtual ~DoublePropertyShadingAmbient() {}
  virtual QString getDisplayName() const { return "Ambient"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 1.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the diffuse parameter for the shading.
 */
class cxGui_EXPORT DoublePropertyShadingDiffuse : public DoublePropertyShadingBase
{
  Q_OBJECT
public:
  DoublePropertyShadingDiffuse(ActiveDataPtr activeData);
  virtual ~DoublePropertyShadingDiffuse() {}
  virtual QString getDisplayName() const { return "Diffuse"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 1.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the specular parameter for the shading.
 */
class cxGui_EXPORT DoublePropertyShadingSpecular : public DoublePropertyShadingBase
{
  Q_OBJECT
public:
  DoublePropertyShadingSpecular(ActiveDataPtr activeData);
  virtual ~DoublePropertyShadingSpecular() {}
  virtual QString getDisplayName() const { return "Specular"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 4.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the specular power parameter for the shading.
  */
class cxGui_EXPORT DoublePropertyShadingSpecularPower : public DoublePropertyShadingBase
{
  Q_OBJECT
public:
  DoublePropertyShadingSpecularPower(ActiveDataPtr activeData);
  virtual ~DoublePropertyShadingSpecularPower() {}
  virtual QString getDisplayName() const { return "Specular Power"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 50.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};
  
/**
 * @}
 */
}// namespace cx
#endif //CXSHADINGPARAMSINTERFACES_H_
