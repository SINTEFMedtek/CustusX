/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTOOLMANUALCALIBRATIONWIDGET_H_
#define CXTOOLMANUALCALIBRATIONWIDGET_H_

#include "org_custusx_calibration_Export.h"

#include "cxBaseWidget.h"
#include "cxTransform3DWidget.h"

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;
/**
 * \file
 * \addtogroup org_custusx_calibration
 * @{
 */

/**Widget for manually changing the tool calibration matrix sMt.
 *
 */
class org_custusx_calibration_EXPORT ToolManualCalibrationWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolManualCalibrationWidget(VisServicesPtr services, QWidget* parent);
  virtual ~ToolManualCalibrationWidget() {}

private slots:
  void toolCalibrationChanged();
  void matrixWidgetChanged();

private:
  QGroupBox* mGroup;
  Transform3DWidget* mMatrixWidget;
  StringPropertySelectToolPtr mTool;
  VisServicesPtr mServices;
};


/**
 * @}
 */
}

#endif /* CXTOOLMANUALCALIBRATIONWIDGET_H_ */
