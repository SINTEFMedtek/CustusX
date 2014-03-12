/*
 * cxCalibrationMethodsWidget.h
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#ifndef CXCALIBRATIONMETHODSWIDGET_H_
#define CXCALIBRATIONMETHODSWIDGET_H_

#include "cxTabbedWidget.h"

#include "cxAcquisitionData.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_calibration
 * @{
 */

class CalibrationMethodsWidget : public TabbedWidget
{
public:
  CalibrationMethodsWidget(AcquisitionDataPtr acquisitionData,
  		QWidget* parent, QString objectName, QString windowTitle);
  virtual ~CalibrationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------

/**
 * @}
 */
}

#endif /* CXCALIBRATIONMETHODSWIDGET_H_ */
