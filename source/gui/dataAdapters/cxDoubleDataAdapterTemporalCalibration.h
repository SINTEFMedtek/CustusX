/*
 * cxDoubleDataAdapterTemporalCalibration.h
 *
 *  Created on: May 5, 2011
 *      Author: christiana
 */

#ifndef CXDOUBLEDATAADAPTERTEMPORALCALIBRATION_H_
#define CXDOUBLEDATAADAPTERTEMPORALCALIBRATION_H_

#include "sscDoubleDataAdapter.h"
#include "sscForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/** Interface to the tool offset of the dominant tool
 */
class DoubleDataAdapterTimeCalibration : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  static ssc::DoubleDataAdapterPtr New();
  DoubleDataAdapterTimeCalibration();
  virtual ~DoubleDataAdapterTimeCalibration() {}
  virtual QString getValueName() const { return "Temporal Calibration"; }
  virtual double getValue() const;
  virtual QString getHelp() const;
  virtual bool setValue(double val);
  ssc::DoubleRange getValueRange() const;

private slots:
  void dominantToolChanged();

private:
  ssc::ToolPtr mTool;
};

/**
 * @}
 */
}

#endif /* CXDOUBLEDATAADAPTERTEMPORALCALIBRATION_H_ */
