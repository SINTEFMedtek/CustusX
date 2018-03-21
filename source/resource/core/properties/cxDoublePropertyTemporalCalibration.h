/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXDOUBLEPROPERTYTEMPORALCALIBRATION_H_
#define CXDOUBLEPROPERTYTEMPORALCALIBRATION_H_

#include "cxResourceExport.h"

#include "cxDoublePropertyBase.h"
#include "cxForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Interface to the tool offset of the active tool
 */
class cxResource_EXPORT DoublePropertyTimeCalibration : public DoublePropertyBase
{
  Q_OBJECT
public:
  static DoublePropertyBasePtr New(TrackingServicePtr trackingService);
  DoublePropertyTimeCalibration(TrackingServicePtr trackingService);
  virtual ~DoublePropertyTimeCalibration() {}
  virtual QString getDisplayName() const { return "Temporal Calibration"; }
  virtual double getValue() const;
  virtual QString getHelp() const;
  virtual bool setValue(double val);
  DoubleRange getValueRange() const;

private slots:
  void activeToolChanged();

private:
  TrackingServicePtr mTrackingService;
  ToolPtr mTool;
};

/**
 * @}
 */
}

#endif /* CXDOUBLEPROPERTYTEMPORALCALIBRATION_H_ */
