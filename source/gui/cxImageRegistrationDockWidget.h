#ifndef CXIMAGEREGISTRATIONDOCKWIDGET_H_
#define CXIMAGEREGISTRATIONDOCKWIDGET_H_

#include <QDockWidget>
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
class MessageManager;

class ImageRegistrationDockWidget : public QDockWidget
{
  typedef ssc::DataManager DataManager;

  Q_OBJECT

public:
  ImageRegistrationDockWidget();
  ~ImageRegistrationDockWidget();

protected slots:
  void addPointButtonClickedSlot();
  void removePointButtonClickedSlot();
  void imageSelectedSlot(const QString& comboBoxText);
  void visibilityOfDockWidgetChangedSlot(bool visible);


protected:
  void populateTheImageComboBox();
  void populateTheLandmarkTableWidget(ssc::ImagePtr image);

  QWidget* mGuiContainer;
  QVBoxLayout* mVerticalLayout;
  QComboBox* mImagesComboBox;
  QTableWidget* mLandmarkTableWidget;
  QPushButton* mAddPointButton;
  QPushButton* mRemovePointButton;

  RepManager* mRepManager;
  DataManager* mDataManager;
  ViewManager* mViewManager;
  MessageManager& mMessageManager;
};
}//namespace cx

#endif /* CXIMAGEREGISTRATIONDOCKWIDGET_H_ */
