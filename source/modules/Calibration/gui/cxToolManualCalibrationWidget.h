/*
 * cxToolManualCalibrationWidget.h
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#ifndef CXTOOLMANUALCALIBRATIONWIDGET_H_
#define CXTOOLMANUALCALIBRATIONWIDGET_H_

#include "cxBaseWidget.h"
#include "cxTransform3DWidget.h"

namespace cx
{
typedef boost::shared_ptr<class SelectToolStringDataAdapter> SelectToolStringDataAdapterPtr;
/**
 * \file
 * \addtogroup cx_module_calibration
 * @{
 */

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
  SelectToolStringDataAdapterPtr mTool;
};


/**
 * @}
 */
}

#endif /* CXTOOLMANUALCALIBRATIONWIDGET_H_ */
