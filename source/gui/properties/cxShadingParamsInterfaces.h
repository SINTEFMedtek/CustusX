/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
