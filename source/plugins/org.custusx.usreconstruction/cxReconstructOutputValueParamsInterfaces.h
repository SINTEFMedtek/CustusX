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
