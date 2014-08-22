/*
 * cxDoubleDataAdapterTemporalCalibration.h
 *
 *  \date May 5, 2011
 *      \author christiana
 */

#ifndef CXDOUBLEDATAADAPTERTEMPORALCALIBRATION_H_
#define CXDOUBLEDATAADAPTERTEMPORALCALIBRATION_H_

#include "cxDoubleDataAdapter.h"
#include "cxForwardDeclarations.h"
#include "cxLegacySingletons.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Interface to the tool offset of the dominant tool
 */
class DoubleDataAdapterTimeCalibration : public DoubleDataAdapter
{
  Q_OBJECT
public:
  static DoubleDataAdapterPtr New();
  DoubleDataAdapterTimeCalibration();
  virtual ~DoubleDataAdapterTimeCalibration() {}
  virtual QString getDisplayName() const { return "Temporal Calibration"; }
  virtual double getValue() const;
  virtual QString getHelp() const;
  virtual bool setValue(double val);
  DoubleRange getValueRange() const;

private slots:
  void dominantToolChanged();

private:
  ToolPtr mTool;
};

/**
 * @}
 */
}

#endif /* CXDOUBLEDATAADAPTERTEMPORALCALIBRATION_H_ */
