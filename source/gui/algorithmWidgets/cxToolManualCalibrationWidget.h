/*
 * cxToolManualCalibrationWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXTOOLMANUALCALIBRATIONWIDGET_H_
#define CXTOOLMANUALCALIBRATIONWIDGET_H_

#include "cxBaseWidget.h"

namespace cx
{

class ToolManualCalibrationWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolManualCalibrationWidget(QWidget* parent);
  virtual ~ToolManualCalibrationWidget() {}
  virtual QString defaultWhatsThis() const;

private slots:

private:
};


}

#endif /* CXTOOLMANUALCALIBRATIONWIDGET_H_ */
