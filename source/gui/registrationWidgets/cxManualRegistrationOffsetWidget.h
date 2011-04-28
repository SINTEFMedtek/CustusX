#ifndef CXMANUALREGISTRATIONOFFSETWIDGET_H_
#define CXMANUALREGISTRATIONOFFSETWIDGET_H_

#include "cxWhatsThisWidget.h"

class QLabel;
class QGridLayout;
class QSlider;
class QSpinBox;
class QPushButton;
class QVBoxLayout;

namespace cx
{
/**
 * \class ManualRegistrationOffsetWidget
 *
 * \brief
 *
 * \date 22. sep. 2010
 * \author: Janne Beate Bakeng
 */

class ManualRegistrationOffsetWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  ManualRegistrationOffsetWidget(QWidget* parent);
  ~ManualRegistrationOffsetWidget();
  virtual QString defaultWhatsThis() const;

protected slots:
  void resetOffsetSlot();
  void setOffsetSlot(int value);
  void activateManualRegistrationFieldSlot(); ///< activates the manuall offset functionality

private:
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QLabel* mOffsetLabel; ///< header label for the offset section
  QGridLayout* mOffsetsGridLayout; ///< layout to put the offset objects into
  QLabel* mXLabel; ///< the text X
  QLabel* mYLabel; ///< the text Y
  QLabel* mZLabel; ///< the text Z
  QSlider* mXOffsetSlider; ///< slider for the user to manually adjust a patient registration with an offset
  QSlider* mYOffsetSlider; ///< slider for the user to manually adjust a patient registration with an offset
  QSlider* mZOffsetSlider; ///< slider for the user to manually adjust a patient registration with an offset
  QSpinBox* mXSpinBox; ///< box for showing X value
  QSpinBox* mYSpinBox; ///< box for showing Y value
  QSpinBox* mZSpinBox; ///< box for showing Z value
  QPushButton* mResetOffsetButton; ///< button for resetting the offset to zero

  int mMinValue, mMaxValue, mDefaultValue; ///< values for the range of the offset
};
}//namespace cx
#endif /* CXMANUALREGISTRATIONOFFSETWIDGET_H_ */
