#ifndef CXPATIENTREGISTRATIONDOCKWIDGET_H_
#define CXPATIENTREGISTRATIONDOCKWIDGET_H_

#include <QWidget>
#include "sscImage.h"
#include "sscTransform3D.h"
#include "cxTool.h"

/**
 * cxPatientRegistrationDockWidget.h
 *
 * \brief
 *
 * \date Feb 3, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QString;
class QLabel;

namespace ssc {
  class DataManager;
}

namespace cx
{
typedef ssc::DataManager DataManager;
typedef ssc::Transform3D Transform3D;
typedef boost::shared_ptr<ssc::Vector3D> Vector3DPtr;

class RegistrationManager;
class ToolManager;
class MessageManager;
class ViewManager;
class RepManager;

class PatientRegistrationWidget : public QWidget
{
  Q_OBJECT

public:
  PatientRegistrationWidget(); ///<
  ~PatientRegistrationWidget(); ///<

protected slots:
  //void imageSelectedSlot(const QString& comboBoxText); //TODO REMOVE
  void currentImageChangedSlot(ssc::ImagePtr currentImage); ///<
  void imageLandmarksUpdateSlot(double, double, double,unsigned int); ///<
  //void visibilityOfWidgetChangedSlot(bool visible);
  void toolSampledUpdateSlot(double, double, double,unsigned int); ///<
  void toolVisibleSlot(bool visible); ///<
  void toolSampleButtonClickedSlot(); ///<
  void rowSelectedSlot(int row, int column); ///<
  //void populateTheImageComboBox(); //TODO REMOVE
  void cellChangedSlot(int row, int column); ///<
  void dominantToolChangedSlot(const std::string& uid); ///<

protected:
  void populateTheLandmarkTableWidget(ssc::ImagePtr image); ///<
  void updateAccuracy(); ///<
  void doPatientRegistration(); ///<

  QVBoxLayout* mVerticalLayout; ///<
  //QComboBox* mImagesComboBox; //TODO REMOVE
  QTableWidget* mLandmarkTableWidget; ///<
  QPushButton* mToolSampleButton; ///<
  QLabel* mAccuracyLabel; ///<

  DataManager* mDataManager; ///<
  RegistrationManager* mRegistrationManager; ///<
  ToolManager* mToolManager; ///<
  MessageManager* mMessageManager; ///<
  ViewManager* mViewManager; ///<
  RepManager* mRepManager; ///<

  int mCurrentRow, mCurrentColumn; ///<
  std::map<int, bool> mLandmarkActiveMap; ///<
  std::map<int, double> mLandmarkRegistrationAccuracyMap; ///<
  double mTotalRegistrationAccuracy; ///<

  ToolPtr mToolToSample; ///<
  ssc::ImagePtr mCurrentImage; ///<

};
}//namespace cx

#endif /* CXPATIENTREGISTRATIONDOCKWIDGET_H_ */
