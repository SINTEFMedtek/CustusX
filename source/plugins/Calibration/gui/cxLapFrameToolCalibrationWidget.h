// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXLAPFRAMETOOLCALIBRATIONWIDGET_H_
#define CXLAPFRAMETOOLCALIBRATIONWIDGET_H_

#include "sscTransform3D.h"
#include "sscVector3D.h"
#include "cxBaseWidget.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"
#include "sscDoubleDataAdapterXml.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_calibration
 * @{
 */


/**
 * Class that handles the tooltip calibration.
 *
 * \date Feb 8, 2012
 * \author Christian Askeland, SINTEF
 */
class LapFrameToolCalibrationWidget : public BaseWidget
{
  Q_OBJECT

public:
  LapFrameToolCalibrationWidget(QWidget* parent);
  virtual ~LapFrameToolCalibrationWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void calibrateSlot();
  void testCalibrationSlot();
  void toolSelectedSlot();
  void trackingStartedSlot();

private:
  QPushButton* mCalibrateButton;
//  LabeledComboBoxWidget* mCalibrateToolComboBox;
  QLabel* mReferencePointLabel;
  QPushButton* mTestButton;
  QLabel* mCalibrationLabel;
  QLabel* mDeltaLabel;
  SelectToolStringDataAdapterPtr mCalibRefTool;
  SelectToolStringDataAdapterPtr mCalibratingTool;
  DoubleDataAdapterXmlPtr mCameraAngleAdapter;
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
class LapFrameToolCalibrationCalculator
{
public:
  LapFrameToolCalibrationCalculator(ToolPtr tool, ToolPtr calRef, double cameraAngle);
  ~LapFrameToolCalibrationCalculator() {}

  Vector3D get_delta_ref(); ///< how far from the reference point the sampled point is, in pr's coord
  Transform3D get_calibration_sMt(); ///< new calibration matrix for the input tool.

private:
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
