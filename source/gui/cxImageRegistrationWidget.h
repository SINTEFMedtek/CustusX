#ifndef CXIMAGEREGISTRATIONWIDGET_H_
#define CXIMAGEREGISTRATIONWIDGET_H_

#include <map>
#include <QWidget>
#include "sscVolumetricRep.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
class RepManager;
class ViewManager;
class RegistrationManager;
class MessageManager;

/**
 * \class ImageRegistrationWidget
 *
 * \brief Widget used as a tab in the ContextDockWidget for image registration.
 *
 * \date Jan 27, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */
class ImageRegistrationWidget : public QWidget
{

  Q_OBJECT

public:
  typedef std::vector<double> Point3D; ///<
  typedef std::map<int, Point3D> Landmarks; ///<

  ImageRegistrationWidget(QWidget* parent); ///< sets up layout and connects signals and slots
  ~ImageRegistrationWidget(); ///< empty

signals:
  void thresholdChanged(const int value); ///< the value selected by the slider

protected slots:
  void currentImageChangedSlot(ssc::ImagePtr currentImage); ///< listens to the contextdockwidget for when the current image is changed
  void addLandmarkButtonClickedSlot(); ///< reacts when the Add Landmark button is clicked
  void editLandmarkButtonClickedSlot(); ///< reacts when the Edit Landmark button is clicked
  void removeLandmarkButtonClickedSlot(); ///< reacts when the Remove Landmark button is clicked
  void imageLandmarksUpdateSlot(double, double, double,unsigned int); ///< updates the table widget when landmarks are added/edited or removed
  void landmarkSelectedSlot(int row, int column); ///< when a landmark i selected from the table
  void cellChangedSlot(int row,int column); ///< reacts when the user types in a (landmark) name
  void thresholdChangedSlot(int value); ///< reemits the valueChanged signal from the slider

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  void populateTheLandmarkTableWidget(ssc::ImagePtr image); ///< populates the table widget

  //gui
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mLandmarkTableWidget; ///< the table widget presenting the landmarks
  QPushButton* mAddLandmarkButton; ///< the Add Landmark button
  QPushButton* mEditLandmarkButton; ///< the Edit Landmark button
  QPushButton* mRemoveLandmarkButton; ///< the Remove Landmark button
  QLabel*      mThresholdLabel; ///< label for the tresholdslider
  QSlider*     mThresholdSlider; ///< slider for setting the probing treshold

  //manageres
  RepManager* mRepManager; ///< has a pool of reps //TODO remove when removing proberep func!!!
  ViewManager* mViewManager; ///< controls layout of views and has a pool of views
  RegistrationManager* mRegistrationManager; ///< handles image and patient registration
  MessageManager* mMessageManager; ///< takes messages intended for the user

  //data
  ssc::ImagePtr mCurrentImage; ///< the image currently used in image registration
  int mCurrentRow, mCurrentColumn; ///< which row and column are currently the choose ones

private:
  ImageRegistrationWidget(); ///< not implemented
};
}//namespace cx

#endif /* CXIMAGEREGISTRATIONWIDGET_H_ */
