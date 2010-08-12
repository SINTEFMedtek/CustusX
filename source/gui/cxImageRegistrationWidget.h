#ifndef CXIMAGEREGISTRATIONWIDGET_H_
#define CXIMAGEREGISTRATIONWIDGET_H_

#include "cxRegistrationWidget.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
/**
 * \class ImageRegistrationWidget
 *
 * \brief Widget used as a tab in the ContextDockWidget for image registration.
 *
 * \date Jan 27, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */
class ImageRegistrationWidget : public RegistrationWidget
{
  Q_OBJECT

public:

  ImageRegistrationWidget(QWidget* parent); ///< sets up layout and connects signals and slots
  virtual ~ImageRegistrationWidget(); ///< empty

signals:
  void thresholdChanged(const int value); ///< the value selected by the slider

protected slots:
  virtual void activeImageChangedSlot(); ///< listens to the datamanager for when the active image is changed
  void addLandmarkButtonClickedSlot(); ///< reacts when the Add Landmark button is clicked
  void editLandmarkButtonClickedSlot(); ///< reacts when the Edit Landmark button is clicked
  void removeLandmarkButtonClickedSlot(); ///< reacts when the Remove Landmark button is clicked
  virtual void cellClickedSlot(int row, int column); ///< when a landmark i selected from the table
  void thresholdChangedSlot(const int value); ///< reemits the valueChanged signal from the slider

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  virtual ssc::LandmarkMap getTargetLandmarks() const;
  virtual void performRegistration();
  virtual void populateTheLandmarkTableWidget(ssc::ImagePtr image); ///< populates the table widget
  QString getLandmarkName(std::string uid);
  virtual ssc::Transform3D getTargetTransform() const;

  //gui
  QPushButton* mAddLandmarkButton; ///< the Add Landmark button
  QPushButton* mEditLandmarkButton; ///< the Edit Landmark button
  QPushButton* mRemoveLandmarkButton; ///< the Remove Landmark button
  QLabel*      mThresholdLabel; ///< label for the tresholdslider
  QSlider*     mThresholdSlider; ///< slider for setting the probing treshold

private:
  ImageRegistrationWidget(); ///< not implemented
};
}//namespace cx

#endif /* CXIMAGEREGISTRATIONWIDGET_H_ */
