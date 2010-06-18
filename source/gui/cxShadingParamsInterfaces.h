/*
 *  cxShadingParamsInterfaces.h
 *  CustusX3
 *
 *  Created by Ole Vegard Solberg on 6/17/10.
 *  Copyright 2010 SINTEF. All rights reserved.
 *
 */
#ifndef CXSHADINGPARAMSINTERFACES_H_
#define CXSHADINGPARAMSINTERFACES_H_

//#include "sscAbstractInterface.h"
#include "sscHelperWidgets.h"
#include "sscForwardDeclarations.h"


namespace cx
{

/** Superclass for all shading parameters
 */
class DoubleDataInterfaceShadingBase : public ssc::DoubleDataInterface
{
  Q_OBJECT 
public:
  DoubleDataInterfaceShadingBase();
  virtual ~DoubleDataInterfaceShadingBase() {}
  //virtual double getValue() const;
  //virtual bool setValue(double val);
  virtual QString getValueID() const { return ""; }
  virtual void connectValueSignals(bool on) {}  
private slots:
  void activeImageChanged();
protected:
  ssc::ImagePtr mImage;
};
  
/** Interface for setting the ambient parameter for the shading.
 */
class DoubleDataInterfaceShadingAmbient : public DoubleDataInterfaceShadingBase
{
  Q_OBJECT
public:
  DoubleDataInterfaceShadingAmbient() {}
  virtual ~DoubleDataInterfaceShadingAmbient() {}
  virtual QString getValueName() const { return "Ambient"; }
  ssc::DoubleRange getValueRange() const  { return ssc::DoubleRange(0.0, 1.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the diffuse parameter for the shading.
 */
class DoubleDataInterfaceShadingDiffuse : public DoubleDataInterfaceShadingBase
{
  Q_OBJECT
public:
  DoubleDataInterfaceShadingDiffuse() {}
  virtual ~DoubleDataInterfaceShadingDiffuse() {}
  virtual QString getValueName() const { return "Diffuse"; }
  ssc::DoubleRange getValueRange() const  { return ssc::DoubleRange(0.0, 1.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the specular parameter for the shading.
 */
class DoubleDataInterfaceShadingSpecular : public DoubleDataInterfaceShadingBase
{
  Q_OBJECT
public:
  DoubleDataInterfaceShadingSpecular() {}
  virtual ~DoubleDataInterfaceShadingSpecular() {}
  virtual QString getValueName() const { return "Specular"; }
  ssc::DoubleRange getValueRange() const  { return ssc::DoubleRange(0.0, 4.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the specular power parameter for the shading.
  */
class DoubleDataInterfaceShadingSpecularPower : public DoubleDataInterfaceShadingBase
{
  Q_OBJECT
public:
  DoubleDataInterfaceShadingSpecularPower() {}
  virtual ~DoubleDataInterfaceShadingSpecularPower() {}
  virtual QString getValueName() const { return "Specular Power"; }
  ssc::DoubleRange getValueRange() const  { return ssc::DoubleRange(0.0, 50.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};
  
}// namespace cx
#endif //CXSHADINGPARAMSINTERFACES_H_
