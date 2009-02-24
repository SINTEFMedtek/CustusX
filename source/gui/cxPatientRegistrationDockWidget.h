#ifndef CXPATIENTREGISTRATIONDOCKWIDGET_H_
#define CXPATIENTREGISTRATIONDOCKWIDGET_H_

#include <QDockWidget>
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

class RegistrationManager;
class ToolManager;
class MessageManager;
class ViewManager;
class RepManager;

class PatientRegistrationDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  PatientRegistrationDockWidget();
  ~PatientRegistrationDockWidget();

protected slots:
  void imageSelectedSlot(const QString& comboBoxText);
  void visibilityOfDockWidgetChangedSlot(bool visible);
  void toolSampledUpdateSlot(double, double, double,unsigned int);
  void toolVisibleSlot(bool visible);
  void toolSampleButtonClickedSlot();
  void doRegistrationButtonClickedSlot();
  void rowSelectedSlot(int row, int column);
  void populateTheImageComboBox();
  void cellChangedSlot(int row, int column);
  void dominantToolChangedSlot(const std::string& uid);

protected:
  void populateTheLandmarkTableWidget(ssc::ImagePtr image);
  void updateAccuracy();
  void doPatientRegistration();

  QWidget* mGuiContainer;
  QVBoxLayout* mVerticalLayout;
  QComboBox* mImagesComboBox;
  QTableWidget* mLandmarkTableWidget;
  QPushButton* mToolSampleButton;
  QPushButton* mDoRegistrationButton;
  QLabel* mAccuracyLabel;

  DataManager* mDataManager;
  RegistrationManager* mRegistrationManager;
  ToolManager* mToolManager;
  MessageManager* mMessageManager;
  ViewManager* mViewManager;
  RepManager* mRepManager;

  int mCurrentRow, mCurrentColumn;
  std::map<int, bool> mLandmarkActiveMap;

  ToolPtr mToolToSample;
  ssc::ImagePtr mCurrentImage;

};
}//namespace cx

#endif /* CXPATIENTREGISTRATIONDOCKWIDGET_H_ */
