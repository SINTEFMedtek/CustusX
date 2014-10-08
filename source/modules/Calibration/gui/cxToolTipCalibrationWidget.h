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

#ifndef CXTOOLTIPCALIBRATIONWIDGET_H_
#define CXTOOLTIPCALIBRATIONWIDGET_H_

#include "cxPluginCalibrationExport.h"

#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxBaseWidget.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxForwardDeclarations.h"
#include "cxDataInterface.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
class LabeledComboBoxWidget;

/**
 * \file
 * \addtogroup cx_module_calibration
 * @{
 */

/**
 * Class that handles the tooltip calibration.
 *
 * \date 3. nov. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class cxPluginCalibration_EXPORT ToolTipCalibrateWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolTipCalibrateWidget(QWidget* parent);
  ~ToolTipCalibrateWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void calibrateSlot();
  void testCalibrationSlot();
  void toolSelectedSlot();

private:
  QPushButton* mCalibrateButton;
  LabeledComboBoxWidget* mCalibrateToolComboBox;
  QLabel* mReferencePointLabel;
  QPushButton* mTestButton;
  QLabel* mCalibrationLabel;
  QLabel* mDeltaLabel;
  SelectToolStringDataAdapterPtr mTools;
};


/**
 * Class that calibrates the tool using a reference point in ref.
 */
class cxPluginCalibration_EXPORT ToolTipCalibrationCalculator
{
public:
  ToolTipCalibrationCalculator(ToolPtr tool, ToolPtr ref, Vector3D p_t = Vector3D());
  ~ToolTipCalibrationCalculator();

  Vector3D get_delta_ref(); ///< how far from the reference point the sampled point is, in pr's coord
  Transform3D get_calibration_sMt(); ///<

private:
  Vector3D get_sampledPoint_t(); ///< the tools sampled point in tool space
  Vector3D get_sampledPoint_ref(); ///< the tools sampled point in ref space
  Vector3D get_referencePoint_ref(); ///< the ref tools reference point in ref space
  Transform3D get_sMt_new(); ///< the new calibration

  ToolPtr mTool; ///< the tool the sampled point is taken from
  ToolPtr mRef; ///< the tool that contains the reference point we are going to calibrate against
  Vector3D mP_t; ///< the sampled point we are working on
};

/**
 * @}
 */
}//namespace cx
#endif /* CXTOOLTIPCALIBRATIONWIDGET_H_ */
