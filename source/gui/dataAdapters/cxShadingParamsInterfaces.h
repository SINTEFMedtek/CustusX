#ifndef CXSHADINGPARAMSINTERFACES_H_
#define CXSHADINGPARAMSINTERFACES_H_

#include "cxDoubleRange.h"
#include "cxDoubleWidgets.h"
#include "cxForwardDeclarations.h"
#include "cxActiveImageProxy.h"
#include "cxLegacySingletons.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Superclass for all shading parameters
 */
class DoubleDataAdapterShadingBase : public DoubleDataAdapter
{
  Q_OBJECT 
public:
  DoubleDataAdapterShadingBase();
  virtual ~DoubleDataAdapterShadingBase() {}
  //virtual double getValue() const;
  //virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}  
private slots:
  void activeImageChanged();
protected:
  ImagePtr mImage;
  ActiveImageProxyPtr mActiveImageProxy;
};
  
/** Interface for setting the ambient parameter for the shading.
 */
class DoubleDataAdapterShadingAmbient : public DoubleDataAdapterShadingBase
{
  Q_OBJECT
public:
  DoubleDataAdapterShadingAmbient() {}
  virtual ~DoubleDataAdapterShadingAmbient() {}
  virtual QString getValueName() const { return "Ambient"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 1.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the diffuse parameter for the shading.
 */
class DoubleDataAdapterShadingDiffuse : public DoubleDataAdapterShadingBase
{
  Q_OBJECT
public:
  DoubleDataAdapterShadingDiffuse() {}
  virtual ~DoubleDataAdapterShadingDiffuse() {}
  virtual QString getValueName() const { return "Diffuse"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 1.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the specular parameter for the shading.
 */
class DoubleDataAdapterShadingSpecular : public DoubleDataAdapterShadingBase
{
  Q_OBJECT
public:
  DoubleDataAdapterShadingSpecular() {}
  virtual ~DoubleDataAdapterShadingSpecular() {}
  virtual QString getValueName() const { return "Specular"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 4.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};

/** Interface for setting the specular power parameter for the shading.
  */
class DoubleDataAdapterShadingSpecularPower : public DoubleDataAdapterShadingBase
{
  Q_OBJECT
public:
  DoubleDataAdapterShadingSpecularPower() {}
  virtual ~DoubleDataAdapterShadingSpecularPower() {}
  virtual QString getValueName() const { return "Specular Power"; }
  DoubleRange getValueRange() const  { return DoubleRange(0.0, 50.0, 0.01); }
  virtual double getValue() const;
  virtual bool setValue(double val);
};
  
/**
 * @}
 */
}// namespace cx
#endif //CXSHADINGPARAMSINTERFACES_H_
