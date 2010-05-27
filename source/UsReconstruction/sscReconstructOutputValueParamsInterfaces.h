/*
 * sscReconstructOutputValueParamsInterfaces.h
 *
 *  Created on: May 27, 2010
 *      Author: christiana
 */
#ifndef SSCRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_
#define SSCRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_

#include "cxDataInterface.h"
#include "sscReconstructer.h"

namespace ssc
{
typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
class OutputVolumeParams;

/** Abstract interface to setting one of the values in class OutputValueParams.
 *  Sublass to get a concrete class.
 */
class DoubleDataInterfaceOutputValueParams : public DoubleDataInterface
{
  Q_OBJECT
public:
  DoubleDataInterfaceOutputValueParams(ReconstructerPtr reconstructer);
  virtual ~DoubleDataInterfaceOutputValueParams() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual QString getValueID() const { return ""; }
  virtual void connectValueSignals(bool on) {}

private slots:

protected:
  virtual double getValue(OutputVolumeParams* params) const = 0;
  virtual void setValue(OutputVolumeParams* params, double val) = 0;

  ReconstructerPtr mReconstructer;
};

/** Interface to setting max size for reconstructed us volume.
 */
class DoubleDataInterfaceMaxUSVolumeSize : public DoubleDataInterfaceOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataInterfaceMaxUSVolumeSize(ReconstructerPtr reconstructer) : DoubleDataInterfaceOutputValueParams(reconstructer), mFactor(1024*1024) {}
  virtual ~DoubleDataInterfaceMaxUSVolumeSize() {}
  virtual QString getValueName() const { return "Volume Size (Mb)"; }
  virtual double convertInternal2Display(double internal) { return internal/mFactor; } ///< conversion from internal value to display value
  virtual double convertDisplay2Internal(double display) { return display*mFactor; } ///< conversion from internal value to display value
  ssc::DoubleRange getValueRange() const  { return ssc::DoubleRange(mFactor,mFactor*500,mFactor); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getVolumeSize(); }
  virtual void setValue(OutputVolumeParams* params, double val) { params->constrainVolumeSize(val); }

  double mFactor;
};

/** Interface to setting spacing in output volume
 */
class DoubleDataInterfaceSpacing : public DoubleDataInterfaceOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataInterfaceSpacing(ReconstructerPtr reconstructer) : DoubleDataInterfaceOutputValueParams(reconstructer) {}
  virtual ~DoubleDataInterfaceSpacing() {}
  virtual QString getValueName() const { return "Output Spacing (mm)"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(0.001,1,0.001); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getSpacing(); }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setSpacing(val); }
};

/** Interface to setting dim in output volume
 */
class DoubleDataInterfaceXDim : public DoubleDataInterfaceOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataInterfaceXDim(ReconstructerPtr reconstructer) : DoubleDataInterfaceOutputValueParams(reconstructer) {}
  virtual ~DoubleDataInterfaceXDim() {}
  virtual QString getValueName() const { return "X Dim"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[0]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(0, val); }
};
/** Interface to setting dim in output volume
 */
class DoubleDataInterfaceYDim : public DoubleDataInterfaceOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataInterfaceYDim(ReconstructerPtr reconstructer) : DoubleDataInterfaceOutputValueParams(reconstructer) {}
  virtual ~DoubleDataInterfaceYDim() {}
  virtual QString getValueName() const { return "Y Dim"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[1]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(1, val); }
};
/** Interface to setting dim in output volume
 */
class DoubleDataInterfaceZDim : public DoubleDataInterfaceOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataInterfaceZDim(ReconstructerPtr reconstructer) : DoubleDataInterfaceOutputValueParams(reconstructer) {}
  virtual ~DoubleDataInterfaceZDim() {}
  virtual QString getValueName() const { return "Z Dim"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[2]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(2, val); }
};

} // namespace ssc

#endif /* SSCRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_ */
