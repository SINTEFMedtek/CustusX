/*
 * cxRegistrationWidget.h
 *
 *  Created on: Apr 21, 2010
 *      Author: dev
 */

#ifndef CXREGISTRATIONWIDGET_H_
#define CXREGISTRATIONWIDGET_H_


#include <map>
#include <QWidget>
#include "sscVolumetricRep.h"
#include "sscLandmark.h"
#include "sscTransform3D.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
/**
 * \class RegistrationWidget
 * Superclass for Image Registration and Patient Registration
 */
class RegistrationWidget : public QWidget
{
  Q_OBJECT

public:
  RegistrationWidget(QWidget* parent); ///< sets up layout and connects signals and slots
  virtual ~RegistrationWidget(); ///< empty

protected slots:
  virtual void activeImageChangedSlot(); ///< listens to the datamanager for when the active image is changed
  virtual void cellClickedSlot(int row, int column); ///< when a landmark i selected from the table

  void cellChangedSlot(int row,int column); ///< reacts when the user types in a (landmark) name
  void landmarkUpdatedSlot();
  void updateAvarageAccuracyLabel();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  virtual void populateTheLandmarkTableWidget(ssc::ImagePtr image); ///< populates the table widget
  virtual ssc::LandmarkMap getTargetLandmarks() const = 0;
  virtual void performRegistration() = 0;
  virtual ssc::Transform3D getTargetTransform() const = 0;

  void nextRow(); ///< jump to the next line in the tablewidget, updates active landmark and highlighted row
  std::vector<ssc::Landmark> getAllLandmarks() const; ///< get all the landmarks from the image and the datamanager
  QString getLandmarkName(std::string uid);
  double getAccuracy(std::string uid);
  double getAvarageAccuracy();

  //gui
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mLandmarkTableWidget; ///< the table widget presenting the landmarks
  QLabel* mAvarageAccuracyLabel; ///< label showing the average accuracy //TODO

  //data
  std::string mActiveLandmark; ///< uid of surrently selected landmark.
  ssc::ImagePtr mCurrentImage; ///< the image currently used in image registration

private:
  RegistrationWidget(); ///< not implemented
};
}//namespace cx


#endif /* CXREGISTRATIONWIDGET_H_ */
