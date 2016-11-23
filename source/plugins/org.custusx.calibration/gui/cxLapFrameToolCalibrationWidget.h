/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  void useOnlyRotationalPart(Transform3D& transform);
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
