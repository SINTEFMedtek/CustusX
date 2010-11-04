#ifndef CXTOOLTIPCALIBRATIONWIDGET_H_
#define CXTOOLTIPCALIBRATIONWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "sscCoordinateSystemHelpers.h"

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

private:
  QGroupBox* createSampleGroupBox();

  QPushButton* mCalibrateButton;
  QPushButton* mTestButton;
  QPushButton* mSampleButton;
  QLineEdit* mFilenameBox;
  ssc::CoordinateSystem mToCoord;

};
}//namespace cx
#endif /* CXTOOLTIPCALIBRATIONWIDGET_H_ */
