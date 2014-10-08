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
#ifndef CXRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_
#define CXRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_

#include "cxPluginUSReconstructionExport.h"

#include "cxDoubleDataAdapter.h"
#include "cxReconstructionManager.h"

namespace cx
{
typedef boost::shared_ptr<class ReconstructionManager> ReconstructManagerPtr;
class OutputVolumeParams;

/**
 * \file
 * \addtogroup cx_module_usreconstruction
 * @{
 */


/** Abstract interface to setting one of the values in class OutputValueParams.
 *  Sublass to get a concrete class.
 */
class cxPluginUSReconstruction_EXPORT DoubleDataAdapterOutputValueParams : public DoubleDataAdapter
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
class cxPluginUSReconstruction_EXPORT DoubleDataAdapterSpacing : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterSpacing(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterSpacing() {}
  virtual QString getDisplayName() const { return "Spacing Out"; }
  virtual QString getHelp() const { return "Output Volume Spacing (mm)"; }
  DoubleRange getValueRange() const {  return DoubleRange(0.001,10,0.001); }
  virtual int getValueDecimals() const { return 3; } ///< number of relevant decimals in value

protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getSpacing(); }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setSpacing(val); }
};

/** Interface to setting dim in output volume
 */
class cxPluginUSReconstruction_EXPORT DoubleDataAdapterXDim : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterXDim(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterXDim() {}
  virtual QString getDisplayName() const { return "X"; }
  virtual QString getHelp() const { return "X dimension"; }
  DoubleRange getValueRange() const {  return DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[0]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(0, val); }
};
/** Interface to setting dim in output volume
 */
class cxPluginUSReconstruction_EXPORT DoubleDataAdapterYDim : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterYDim(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterYDim() {}
  virtual QString getDisplayName() const { return "Y"; }
  virtual QString getHelp() const { return "Y dimension"; }
  DoubleRange getValueRange() const {  return DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[1]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(1, val); }
};
/** Interface to setting dim in output volume
 */
class cxPluginUSReconstruction_EXPORT DoubleDataAdapterZDim : public DoubleDataAdapterOutputValueParams
{
  Q_OBJECT
public:
  DoubleDataAdapterZDim(ReconstructManagerPtr reconstructer) : DoubleDataAdapterOutputValueParams(reconstructer) {}
  virtual ~DoubleDataAdapterZDim() {}
  virtual QString getDisplayName() const { return "Z"; }
  virtual QString getHelp() const { return "Z dimension"; }
  DoubleRange getValueRange() const {  return DoubleRange(1,1000,1); }
protected:
  virtual double getValue(OutputVolumeParams* params) const { return params->getDim()[2]; }
  virtual void setValue(OutputVolumeParams* params, double val) { params->setDim(2, val); }
};

/**
 * @}
 */
} // namespace cx

#endif /* CXRECONSTRUCTOUTPUTVALUEPARAMSINTERFACES_H_ */
