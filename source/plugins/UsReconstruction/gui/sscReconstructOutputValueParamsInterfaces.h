/*
 * sscReconstructOutputValueParamsInterfaces.h
 *
 *  \date May 27, 2010
 *      \author christiana
 */
#ifndef SSCRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_
#define SSCRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_

#include "sscDoubleDataAdapter.h"
#include "sscReconstructManager.h"

namespace ssc
{
typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
class OutputVolumeParams;

/**
 * \file
 * \addtogroup cxPluginUsReconstruction
 * @{
 */


/** Abstract interface to setting one of the values in class OutputValueParams.
 *  Sublass to get a concrete class.
 */
class DoubleDataAdapterOutputValueParams : public DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterOutputValueParams(ReconstructManagerPtr reconstructer);
  virtual ~DoubleDataAdapterOutputValueParams() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}

private slots:

protected:
  virtual double getValue(OutputVolumeParams* params) const = 0;
  virtual void setValue(OutputVolumeParams* params, double val) = 0;

  ReconstructManagerPtr mReconstructer;
};

/** Interface to setting spacing in output volume
 */
class DoubleDataAdapterSpacing : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterSpacing(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterSpacing() {}
  virtual QString getValueName() const { return "Spacing Out"; }
  virtual QString getHelp() const { return "Output Volume Spacing (mm)"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(0.001,10,0.001); }
  virtual int getValueDecimals() const { return 3; } ///< number of relevant decimals in value

protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getSpacing(); }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setSpacing(val); }
};

/** Interface to setting dim in output volume
 */
class DoubleDataAdapterXDim : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterXDim(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterXDim() {}
  virtual QString getValueName() const { return "X"; }
  virtual QString getHelp() const { return "X dimension"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[0]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(0, val); }
};
/** Interface to setting dim in output volume
 */
class DoubleDataAdapterYDim : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterYDim(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterYDim() {}
  virtual QString getValueName() const { return "Y"; }
  virtual QString getHelp() const { return "Y dimension"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[1]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(1, val); }
};
/** Interface to setting dim in output volume
 */
class DoubleDataAdapterZDim : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterZDim(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterZDim() {}
  virtual QString getValueName() const { return "Z"; }
  virtual QString getHelp() const { return "Z dimension"; }
  DoubleRange getValueRange() const {  return ssc::DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[2]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(2, val); }
};

/**
 * @}
 */
} // namespace ssc

#endif /* SSCRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_ */
