#ifndef CXPATIENTREGISTRATIONWIDGET_H_
#define CXPATIENTREGISTRATIONWIDGET_H_

#include <QWidget>
#include <sscImage.h>
#include <sscTransform3D.h>
#include "cxTool.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QString;
class QLabel;
class QSlider;
class QGridLayout;
class QSpinBox;

namespace cx
{
typedef ssc::Transform3D Transform3D;
typedef boost::shared_ptr<ssc::Vector3D> Vector3DPtr;

class RegistrationManager;
class ToolManager;
class MessageManager;
class ViewManager;
class RepManager;

/**
 * \class PatientRegistrationWidget
 *
 * \brief Widget used as a tab in the ContexDockWidget for patient registration.
 *
 * \date Feb 3, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */
class PatientRegistrationWidget : public QWidget
{
  Q_OBJECT

public:
  PatientRegistrationWidget(QWidget* parent); ///< sets up layout and connects signals and slots
  ~PatientRegistrationWidget(); ///< empty

protected slots:
  void currentImageChangedSlot(ssc::ImagePtr currentImage); ///< listens to the contextdockwidget for when the current image is changed
  void imageLandmarksUpdateSlot(double, double, double,unsigned int); ///< updates the table widget when landmarks are added/edited or removed
  void toolSampledUpdateSlot(double, double, double,unsigned int); ///<
  void toolVisibleSlot(bool visible); ///< enables/disables the Sample Tool button
  void toolSampleButtonClickedSlot(); ///< reacts when the Sample Tool button is clicked
  void rowSelectedSlot(int row, int column); ///<  updates the current row and column
  void cellChangedSlot(int row, int column); ///<  reacts when the user types in a (landmark) name
  void dominantToolChangedSlot(const std::string& uid); ///< set which tool to sample from
  void resetOffsetSlot(); ///< resets the patient registration offset to zero
  void setOffsetSlot(int value); ///< set the patient registration offset
  void activateManualRegistrationFieldSlot(); ///< activates the manuall offset functionality

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  void populateTheLandmarkTableWidget(ssc::ImagePtr image); ///< populates the table widget
  void updateAccuracy(); ///< calculates accuracy for each landmark after a registration
  void doPatientRegistration(); ///< initializes patient registration

  //gui
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mLandmarkTableWidget; ///< the table widget presenting the landmarks
  QPushButton* mToolSampleButton; ///< the Sample Tool button
  QLabel* mAvarageAccuracyLabel; ///< label showing the average accuracy //TODO
  QLabel* mOffsetLabel; ///< header label for the offset section
  QWidget* mOffsetWidget; ///< widget for offset functionality
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

  //managers
  RegistrationManager* mRegistrationManager; ///< handles image and patient registration
  ToolManager* mToolManager; ///< interface to the navigation system
  MessageManager* mMessageManager; ///< takes messages intended for the user
  ViewManager* mViewManager; ///< controls layout of views and has a pool of views
  RepManager* mRepManager; ///< has a pool of reps

  //data
  int mCurrentRow, mCurrentColumn; ///< which row and column are currently the choose ones
  std::map<int, double> mLandmarkRegistrationAccuracyMap; ///< maps accuracy to index of a landmark
  double mAverageRegistrationAccuracy; ///< the average registration accuracy of the last registration
  ToolPtr mToolToSample; ///< tool to be sampled from
  ssc::ImagePtr mCurrentImage; ///< the image currently used in image registration

  int mMinValue, mMaxValue, mDefaultValue; ///< values for the range of the offset

private:
  PatientRegistrationWidget(); ///< not implemented

};
}//namespace cx

#endif /* CXPATIENTREGISTRATIONWIDGET_H_ */
