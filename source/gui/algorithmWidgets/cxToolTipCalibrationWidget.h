#ifndef CXTOOLTIPCALIBRATIONWIDGET_H_
#define CXTOOLTIPCALIBRATIONWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
/**
 * \class ToolTipCalibrationWidget
 *
 * \brief
 *
 * \date 3. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class ToolTipCalibrationWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  ToolTipCalibrationWidget(QWidget* parent);
  ~ToolTipCalibrationWidget();
  virtual QString defaultWhatsThis() const;

private:
  class ToolTipCalibrateWidget* mCalibrationWidget;
  class ToolTipSampleWidget*      mSampleWidget;
};

/**
 * Class that handles the tooltip calibration.
 */
class ToolTipCalibrateWidget : public WhatsThisWidget
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
  ssc::LabeledComboBoxWidget* mCoordinateSystemComboBox;
  ssc::LabeledComboBoxWidget* mToolComboBox;
};

/**
 * Class that handles the tooltip calibration.
 */
class ToolTipSampleWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  ToolTipSampleWidget(QWidget* parent);
  ~ToolTipSampleWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void saveFileSlot();
  void sampleSlot();
  void coordinateSystemChanged();

private:
  ssc::CoordinateSystem getSelectedCoordinateSystem();

  QPushButton* mSampleButton;
  QLabel*      mSaveToFileNameLabel;
  QPushButton* mSaveFileButton;
  SelectCoordinateSystemStringDataAdapterPtr mCoordinateSystems;
  SelectToolStringDataAdapterPtr mTools;
  SelectDataStringDataAdapterPtr mData;
  ssc::LabeledComboBoxWidget* mCoordinateSystemComboBox;
  ssc::LabeledComboBoxWidget* mToolComboBox;
  ssc::LabeledComboBoxWidget* mDataComboBox;
  bool mTruncateFile;
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

}//namespace cx
#endif /* CXTOOLTIPCALIBRATIONWIDGET_H_ */
