/*
 * cxTemporalCalibrationWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXTEMPORALCALIBRATIONWIDGET_H_
#define CXTEMPORALCALIBRATIONWIDGET_H_

#include "cxBaseWidget.h"

namespace cx
{

class TemporalCalibrationWidget : public BaseWidget
{
  Q_OBJECT

public:
  TemporalCalibrationWidget(QWidget* parent);
  virtual ~TemporalCalibrationWidget() {}
  virtual QString defaultWhatsThis() const;

private slots:

private:
};


}

#endif /* CXTEMPORALCALIBRATIONWIDGET_H_ */
