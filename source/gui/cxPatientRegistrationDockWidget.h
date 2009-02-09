#ifndef CXPATIENTREGISTRATIONDOCKWIDGET_H_
#define CXPATIENTREGISTRATIONDOCKWIDGET_H_

#include <QDockWidget>
#include "sscImage.h"
#include "sscTransform3D.h"

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

class PatientRegistrationDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  PatientRegistrationDockWidget();
  ~PatientRegistrationDockWidget();

protected slots:
  void imageSelectedSlot(const QString& comboBoxText);
  void visibilityOfDockWidgetChangedSlot(bool visible);
  void toolVisibleSlot(bool visible);
  void toolTransformAndTimestamp(Transform3D transform, double timestamp);
  
  void landmarkSelectedSlot(int row, int column);
  void populateTheImageComboBox();

protected:
  void populateTheLandmarkTableWidget(ssc::ImagePtr image);

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
  MessageManager& mMessageManager;

  int mCurrentRow, mCurrentColumn;
};
}//namespace cx

#endif /* CXPATIENTREGISTRATIONDOCKWIDGET_H_ */
