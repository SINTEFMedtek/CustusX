/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLTIPCALIBRATIONWIDGET_H_
#define CXTOOLTIPCALIBRATIONWIDGET_H_

#include "org_custusx_calibration_Export.h"

#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxBaseWidget.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxForwardDeclarations.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
class LabeledComboBoxWidget;
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
 * \date 3. nov. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_calibration_EXPORT ToolTipCalibrateWidget : public BaseWidget
{
	Q_OBJECT

public:
	ToolTipCalibrateWidget(VisServicesPtr services, QWidget* parent);
	~ToolTipCalibrateWidget();

private slots:
	void calibrateSlot();
	void testCalibrationSlot();
	void toolSelectedSlot();
	void onTrackingSystemStateChanged();

private:
	VisServicesPtr mServices;
	QPushButton* mCalibrateButton;
	LabeledComboBoxWidget* mCalibrateToolComboBox;
	QLabel* mReferencePointLabel;
	QPushButton* mTestButton;
	QLabel* mCalibrationLabel;
	QLabel* mDeltaLabel;
	StringPropertySelectToolPtr mTools;
};


/**
 * Class that calibrates the tool using a reference point in ref.
 */
class org_custusx_calibration_EXPORT ToolTipCalibrationCalculator
{
public:
	ToolTipCalibrationCalculator(SpaceProviderPtr spaces, ToolPtr tool, ToolPtr ref, Vector3D p_t = Vector3D());
	~ToolTipCalibrationCalculator();

	Vector3D get_delta_ref(); ///< how far from the reference point the sampled point is, in pr's coord
	Transform3D get_calibration_sMt(); ///<

private:
	Vector3D get_sampledPoint_t(); ///< the tools sampled point in tool space
	Vector3D get_sampledPoint_ref(); ///< the tools sampled point in ref space
	Vector3D get_referencePoint_ref(); ///< the ref tools reference point in ref space
	Transform3D get_sMt_new(); ///< the new calibration

	SpaceProviderPtr mSpaces;
	ToolPtr mTool; ///< the tool the sampled point is taken from
	ToolPtr mRef; ///< the tool that contains the reference point we are going to calibrate against
	Vector3D mP_t; ///< the sampled point we are working on
};

/**
 * @}
 */
}//namespace cx
#endif /* CXTOOLTIPCALIBRATIONWIDGET_H_ */
