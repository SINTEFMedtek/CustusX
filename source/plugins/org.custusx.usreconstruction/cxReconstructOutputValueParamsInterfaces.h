/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_
#define CXRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_

#include "org_custusx_usreconstruction_Export.h"

#include "cxDoublePropertyBase.h"

namespace cx
{
//typedef boost::shared_ptr<class ReconstructionManager> ReconstructManagerPtr;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
class OutputVolumeParams;

/**
 * \file
 * \addtogroup org_custusx_usreconstruction
 * @{
 */


/** Abstract interface to setting one of the values in class OutputValueParams.
 *  Sublass to get a concrete class.
 */
class org_custusx_usreconstruction_EXPORT DoublePropertyOutputValueParams : public DoublePropertyBase
{
  Q_OBJECT
public:
  DoublePropertyOutputValueParams(UsReconstructionServicePtr reconstructer);
  virtual ~DoublePropertyOutputValueParams() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}

private slots:

protected:
  virtual double getValue(OutputVolumeParams* params) const = 0;
  virtual void setValue(OutputVolumeParams* params, double val) = 0;

  UsReconstructionServicePtr mReconstructer;
};

/** Interface to setting spacing in output volume
 */
class org_custusx_usreconstruction_EXPORT DoublePropertySpacing : public DoublePropertyOutputValueParams
{
  Q_OBJECT
public:
  DoublePropertySpacing(UsReconstructionServicePtr reconstructer) : DoublePropertyOutputValueParams(reconstructer) {}
  virtual ~DoublePropertySpacing() {}
  virtual QString getDisplayName() const { return "Spacing Out"; }
  virtual QString getHelp() const { return "Output Volume Spacing (mm)"; }
  DoubleRange getValueRange() const {  return DoubleRange(0.001,10,0.001); }
  virtual int getValueDecimals() const { return 3; } ///< number of relevant decimals in value

protected:
  virtual double getValue(OutputVolumeParams* params) const;
  virtual void setValue(OutputVolumeParams* params, double val);
};

/** Interface to setting dim in output volume
 */
class org_custusx_usreconstruction_EXPORT DoublePropertyXDim : public DoublePropertyOutputValueParams
{
  Q_OBJECT
public:
  DoublePropertyXDim(UsReconstructionServicePtr reconstructer) : DoublePropertyOutputValueParams(reconstructer) {}
  virtual ~DoublePropertyXDim() {}
  virtual QString getDisplayName() const { return "X"; }
  virtual QString getHelp() const { return "X dimension"; }
  DoubleRange getValueRange() const {  return DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const;
  virtual void setValue(OutputVolumeParams* params, double val);
};
/** Interface to setting dim in output volume
 */
class org_custusx_usreconstruction_EXPORT DoublePropertyYDim : public DoublePropertyOutputValueParams
{
  Q_OBJECT
public:
  DoublePropertyYDim(UsReconstructionServicePtr reconstructer) : DoublePropertyOutputValueParams(reconstructer) {}
  virtual ~DoublePropertyYDim() {}
  virtual QString getDisplayName() const { return "Y"; }
  virtual QString getHelp() const { return "Y dimension"; }
  DoubleRange getValueRange() const {  return DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const;
  virtual void setValue(OutputVolumeParams* params, double val);
};
/** Interface to setting dim in output volume
 */
class org_custusx_usreconstruction_EXPORT DoublePropertyZDim : public DoublePropertyOutputValueParams
{
  Q_OBJECT
public:
  DoublePropertyZDim(UsReconstructionServicePtr reconstructer) : DoublePropertyOutputValueParams(reconstructer) {}
  virtual ~DoublePropertyZDim() {}
  virtual QString getDisplayName() const { return "Z"; }
  virtual QString getHelp() const { return "Z dimension"; }
  DoubleRange getValueRange() const {  return DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const;
  virtual void setValue(OutputVolumeParams* params, double val);
};

/**
 * @}
 */
} // namespace cx

#endif /* CXRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_ */
