#ifndef CXTOOLTIPCALIBRATIONWIDGET_H_
#define CXTOOLTIPCALIBRATIONWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"

class QShowEvent;
class QCloseEvent;
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

protected:
  virtual void showEvent(QShowEvent* event);
  virtual void hideEvent(QCloseEvent* event);

private slots:
  void calibrateSlot();
  void testSlot();
  void sampleSlot();
  void coordChangedSlot();
  void referenceObjectChanged();
  void saveFileSlot();

private:
  QGroupBox* createCalibrateGroupBox();
  QGroupBox* createTestGroupBox();
  QGroupBox* createSampleGroupBox();

  QPushButton* mCalibrateButton;
  QPushButton* mTestButton;
  QPushButton* mSampleButton;
  QLabel*      mSaveToFileNameLabel;
  QPushButton* mSaveFileButton;
  ssc::CoordinateSystem mToCoordinateSystem;
  SelectCoordinateSystemStringDataAdapterPtr mCoordinateSystems;
  SelectToolStringDataAdapterPtr mTools;
  SelectDataStringDataAdapterPtr mData;
  ssc::LabeledComboBoxWidget* mCoordinateSystemComboBox;
  ssc::LabeledComboBoxWidget* mToolComboBox;
  ssc::LabeledComboBoxWidget* mDataComboBox;

};
}//namespace cx
#endif /* CXTOOLTIPCALIBRATIONWIDGET_H_ */
