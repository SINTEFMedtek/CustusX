#ifndef CXNAVIGATIONWIDGET_H_
#define CXNAVIGATIONWIDGET_H_

#include "cxBaseWidget.h"

class QGroupBox;
class QRadioButton;
class QVBoxLayout;
class QLabel;
class QSlider;

#include "cxCameraStyle.h"

namespace cx
{

/**
 * \class NavigationWidget
 *
 * \brief Widget for controlling camera follow style.
 * \ingroup cxGUI
 *
 * \date 22. feb. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class NavigationWidget : public BaseWidget
{
  Q_OBJECT

public:
  NavigationWidget(QWidget* parent);
  ~NavigationWidget();

  virtual QString defaultWhatsThis() const;

//protected slots:
//  void radioButtonToggledSlot(bool checked);
//  void trackingConfiguredSlot();

protected:
  QVBoxLayout*  mVerticalLayout; ///< vertical layout is used

  //camera settings
  QGroupBox*    mCameraGroupBox; ///< widget to put all camera specific settings
//  QRadioButton* mDefaultCameraStyleRadioButton; ///< for selecting the default camera style
//  QRadioButton* mToolCameraStyleRadioButton; ///< for selecting tool following camera style
//  QRadioButton* mAngledToolCameraStyleRadioButton; ///< for selecting an angled tool following camera style
//  QLabel*       mCameraOffsetLabel; ///< camera offset label
//  QSlider*      mCameraOffsetSlider; ///< slider for adjusting the camera offset
  QVBoxLayout*  mCameraGroupLayout; ///< layout for the cameragroupbox

//  CameraStylePtr mCameraStyle;
  CameraStylePtr getCameraStyle();
};
}
#endif /* CXNAVIGATIONWIDGET_H_ */
