#ifndef CXNAVIGATIONWIDGET_H_
#define CXNAVIGATIONWIDGET_H_

#include <QWidget>

class QGroupBox;
class QRadioButton;
class QVBoxLayout;
class QLabel;
class QSlider;

namespace cx
{

/**
 * \class cxNavigationWidget.h
 *
 * \brief
 *
 * \date 22. feb. 2010
 * \author: jbake
 */
class NavigationWidget : public QWidget
{
  Q_OBJECT

public:
  NavigationWidget(QWidget* parent); ///<
  ~NavigationWidget(); ///< empty

protected slots:
  void radioButtonToggledSlot(bool checked);
  void trackingConfiguredSlot();
  void trackingInitializedSlot();
  void trackingStartedSlot();
  void trackingStoppedSlot();


protected:
  QVBoxLayout*  mVerticalLayout; ///< vertical layout is used

  //status
  QGroupBox*    mTrackingStatusGroupBox; ///< widget for showing tracking status
  QVBoxLayout*  mTrackingStatusGroupLayout; ///< layout for the trackingstatusgroupbox
  QLabel*       mTrackingStatusLabel; ///< label showing tracking status

  //camera settings
  QGroupBox*    mCameraGroupBox; ///< widget to put all camera specific settings
  QRadioButton* mDefaultCameraStyleRadioButton; ///< for selecting the default camera style
  QRadioButton* mToolCameraStyleRadioButton; ///< for selecting tool following camera style
  QLabel*       mCameraOffsetLabel; ///< camera offset label
  QSlider*      mCameraOffsetSlider; ///< slider for adjusting the camera offset
  QVBoxLayout*  mCameraGroupLayout; ///< layout for the cameragroupbox
};
}
#endif /* CXNAVIGATIONWIDGET_H_ */
