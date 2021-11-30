/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLAPFRAMETOOLCALIBRATIONWIDGET_H_
#define CXLAPFRAMETOOLCALIBRATIONWIDGET_H_

#include "org_custusx_calibration_Export.h"

#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxBaseWidget.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxForwardDeclarations.h"
#include "cxDoubleProperty.h"

class QPushButton;
class QGroupBox;
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
 * \date Feb 8, 2012
 * \author Christian Askeland, SINTEF
 */
class org_custusx_calibration_EXPORT LapFrameToolCalibrationWidget : public BaseWidget
{
	Q_OBJECT

public:
	LapFrameToolCalibrationWidget(VisServicesPtr services, QWidget* parent);
	virtual ~LapFrameToolCalibrationWidget();

private slots:
	void calibrateSlot();
	void testCalibrationSlot();
	void toolSelectedSlot();
	void trackingStartedSlot();

private:
	VisServicesPtr mServices;
	QPushButton* mCalibrateButton;
	QLabel* mReferencePointLabel;
	QPushButton* mTestButton;
	QLabel* mCalibrationLabel;
	QLabel* mDeltaLabel;
	StringPropertySelectToolPtr mCalibRefTool;
	StringPropertySelectToolPtr mCalibratingTool;
	DoublePropertyPtr mCameraAngleAdapter;
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
class org_custusx_calibration_EXPORT LapFrameToolCalibrationCalculator
{
public:
	LapFrameToolCalibrationCalculator(ToolPtr tool, ToolPtr calRef, double cameraAngle);
	~LapFrameToolCalibrationCalculator() {}

	Vector3D get_delta_ref(); ///< how far from the reference point the sampled point is, in pr's coord
	Transform3D get_calibration_sMt(); ///< new calibration matrix for the input tool.

private:
	void useOnlyRotationalPart(Transform3D* transform);
	ToolPtr mTool; ///< the tool the sampled point is taken from
	ToolPtr mCalibrationRef; ///< the tool that contains the reference point we are going to calibrate against
	Transform3D m_sMpr; ///< raw tracking position, patient reference to tool sensor.
	Transform3D m_qMpr; ///< position of calibration position.
	Transform3D m_qMcr; ///< hardcoded position of calibration point relative to calibration tool cr.
	double mCameraAngle; ///< additional y-tilt on the tool side.
};
/**
 * @}
 */
}

#endif /* CXLAPFRAMETOOLCALIBRATIONWIDGET_H_ */
