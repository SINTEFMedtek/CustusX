#ifndef CXIMAGEREGISTRATIONWIDGET_H_
#define CXIMAGEREGISTRATIONWIDGET_H_

#include <map.h>
#include <QWidget>
#include "cxVolumetricRep.h"

/**
 * cxImageRegistrationDockWidget.h
 *
 * \brief This class represents an interface to a dockwidget used for image to
 * image registration.
 *
 * \date Jan 27, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;

namespace ssc
{
class DataManager;
}
namespace cx
{
class RepManager;
class ViewManager;
class RegistrationManager;
class MessageManager;

class ImageRegistrationWidget : public QWidget
{
  typedef ssc::DataManager DataManager;

  Q_OBJECT

public:
  ImageRegistrationWidget();
  ~ImageRegistrationWidget();

protected slots:
  void addLandmarkButtonClickedSlot();
  void editLandmarkButtonClickedSlot();
  void removeLandmarkButtonClickedSlot();
  void imageSelectedSlot(const QString& comboBoxText);
  void visibilityOfDockWidgetChangedSlot(bool visible);
  void imageLandmarksUpdateSlot(double, double, double,unsigned int);
  void landmarkSelectedSlot(int row, int column);
  void populateTheImageComboBox();
  void cellChangedSlot(int row,int column);


protected:
  void populateTheLandmarkTableWidget(ssc::ImagePtr image);

  QVBoxLayout* mVerticalLayout;
  QComboBox* mImagesComboBox;
  QTableWidget* mLandmarkTableWidget;
  QPushButton* mAddLandmarkButton;
  QPushButton* mEditLandmarkButton;
  QPushButton* mRemoveLandmarkButton;

  RepManager* mRepManager;
  DataManager* mDataManager;
  ViewManager* mViewManager;
  RegistrationManager* mRegistrationManager;
  MessageManager* mMessageManager;

  ssc::ImagePtr mCurrentImage;
  std::map<int, bool> mLandmarkActiveMap;

  int mCurrentRow, mCurrentColumn;
};
}//namespace cx

#endif /* CXIMAGEREGISTRATIONWIDGET_H_ */
