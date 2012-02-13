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

#ifndef CXTOOLTIPCALIBRATIONWIDGET_H_
#define CXTOOLTIPCALIBRATIONWIDGET_H_

#include "cxBaseWidget.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginCalibration
 * @{
 */

/**
 * Class that handles the tooltip calibration.
 *
 * \date 3. nov. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class ToolTipCalibrateWidget : public BaseWidget
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
  ssc::LabeledComboBoxWidget* mCalibrateToolComboBox;
  QLabel* mReferencePointLabel;
  QPushButton* mTestButton;
  QLabel* mCalibrationLabel;
  QLabel* mDeltaLabel;
  SelectToolStringDataAdapterPtr mTools;
};


/**
 * Class that calibrates the tool using a reference point in ref.
 */
class ToolTipCalibrationCalculator
{
public:
  ToolTipCalibrationCalculator(ssc::ToolPtr tool, ssc::ToolPtr ref, ssc::Vector3D p_t = ssc::Vector3D());
  ~ToolTipCalibrationCalculator();

  ssc::Vector3D get_delta_ref(); ///< how far from the reference point the sampled point is, in pr's coord
  ssc::Transform3D get_calibration_sMt(); ///<

private:
  ssc::Vector3D get_sampledPoint_t(); ///< the tools sampled point in tool space
  ssc::Vector3D get_sampledPoint_ref(); ///< the tools sampled point in ref space
  ssc::Vector3D get_referencePoint_ref(); ///< the ref tools reference point in ref space
  ssc::Transform3D get_sMt_new(); ///< the new calibration

  ssc::ToolPtr mTool; ///< the tool the sampled point is taken from
  ssc::ToolPtr mRef; ///< the tool that contains the reference point we are going to calibrate against
  ssc::Vector3D mP_t; ///< the sampled point we are working on
};

/**
 * @}
 */
}//namespace cx
#endif /* CXTOOLTIPCALIBRATIONWIDGET_H_ */
