/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXEBUSCALIBRATIONWIDGET_H_
#define CXEBUSCALIBRATIONWIDGET_H_

#include "org_custusx_calibration_Export.h"

#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxBaseWidget.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxForwardDeclarations.h"
#include "cxDoubleProperty.h"

class QPushButton;
class QGroupBox;
class QComboBox;
class QLineEdit;

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

/**
 * \file
 * \addtogroup org_custusx_calibration
 * @{
 */


/**
 * Class that handles the tooltip calibration.
 *
 * \date Oct 2019
 * \author Erlend Hofstad, SINTEF
 */
class org_custusx_calibration_EXPORT EBUSCalibrationWidget : public BaseWidget
{
  Q_OBJECT

public:
	EBUSCalibrationWidget(VisServicesPtr services, QWidget* parent);
	virtual ~EBUSCalibrationWidget();

private slots:
  void calibrateSlot();
  void toolSelectedSlot();
  void trackingStartedSlot();
	void browseAdapterCalibrationPathSlot();

private:
	Transform3D readCalibrationFile(QString absoluteFilePath);

  VisServicesPtr mServices;
  QPushButton* mCalibrateButton;
  QLabel* mReferencePointLabel;
  QLabel* mCalibrationLabel;
  QLabel* mDeltaLabel;
	QString mAdapterCalibrationPath;
	QComboBox* mAdapterCalibrationPathComboBox;
  StringPropertySelectToolPtr mCalibRefTool;
  StringPropertySelectToolPtr mCalibratingTool;
};

/**
 * Class that calibrates the tool using a reference matrix in a calibration tool.
 *
 *  - s: sensor on tool to be calibrated
 *  - t: tool space of tool to be calibrated
 *  - cr: calibration tool, i.e. the tool where the calibration position exist
 *  - q: Position (matrix) of calibration position on the calibration frame.
 *  - pr: patient reference
 */
class org_custusx_calibration_EXPORT EBUSCalibrationCalculator
{
public:
	EBUSCalibrationCalculator(ToolPtr tool, ToolPtr calRef);
	~EBUSCalibrationCalculator() {}

	Transform3D get_calibration_sMt(Transform3D adapterCalibration); ///< new calibration matrix for the input tool.

private:
  ToolPtr mTool; ///< the tool the sampled point is taken from
  ToolPtr mCalibrationRef; ///< the tool that contains the reference point we are going to calibrate against
  Transform3D m_sMpr; ///< raw tracking position, patient reference to tool sensor.
  Transform3D m_qMpr; ///< position of calibration position.
  Transform3D m_qMcr; ///< hardcoded position of calibration point relative to calibration tool cr.
};
/**
 * @}
 */
}

#endif /* CXEBUSCALIBRATIONWIDGET_H_ */
