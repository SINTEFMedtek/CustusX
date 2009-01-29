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

namespace cx
{
class RepManager;

class ImageRegistrationDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  ImageRegistrationDockWidget();
  ~ImageRegistrationDockWidget();

protected slots:
  void volumetricRepSelectedSlot(const QString& comboBoxText);

protected:
  void populateTheImageComboBox();

  QWidget* mGuiContainer;
  QVBoxLayout* mVerticalLayout;
  QComboBox* mImagesComboBox;
  QTableWidget* mLandmarkTableWidget;
  QPushButton* mAddPointButton;
  QPushButton* mRemovePointButton;

  RepManager* mRepManager;
};
}//namespace cx

#endif /* CXIMAGEREGISTRATIONDOCKWIDGET_H_ */
