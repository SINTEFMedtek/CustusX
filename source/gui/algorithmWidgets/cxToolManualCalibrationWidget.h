/*
 * cxToolManualCalibrationWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXTOOLMANUALCALIBRATIONWIDGET_H_
#define CXTOOLMANUALCALIBRATIONWIDGET_H_

#include "cxBaseWidget.h"
#include "cxTransform3DWidget.h"

namespace cx
{

/**Widget for manually changing the tool calibration matrix sMt.
 *
 */
class ToolManualCalibrationWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolManualCalibrationWidget(QWidget* parent);
  virtual ~ToolManualCalibrationWidget() {}
  virtual QString defaultWhatsThis() const;

private slots:
  void toolCalibrationChanged();
  void matrixWidgetChanged();

private:
  QGroupBox* mGroup;
  Transform3DWidget* mMatrixWidget;

};


}

#endif /* CXTOOLMANUALCALIBRATIONWIDGET_H_ */
