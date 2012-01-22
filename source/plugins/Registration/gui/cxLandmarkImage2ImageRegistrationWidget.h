/*
 * cxLandmarkImage2ImageRegistrationWidget.h
 *
 *  Created on: Sep 7, 2011
 *      Author: christiana
 */

#ifndef CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_
#define CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_

#include "cxLandmarkRegistrationWidget.h"
#include "cxRegistrationDataAdapters.h"
#include "Rep/cxLandmarkRep.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**
 * Widget for performing image to image registration
 * using landmarks.
 */
class LandmarkImage2ImageRegistrationWidget : public LandmarkRegistrationWidget
{
  Q_OBJECT

public:

  LandmarkImage2ImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle); ///< sets up layout and connects signals and slots
  virtual ~LandmarkImage2ImageRegistrationWidget(); ///< empty
  virtual QString defaultWhatsThis() const;

protected slots:
  void updateRep();
  void registerSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  virtual ssc::LandmarkMap getTargetLandmarks() const;
  virtual void performRegistration();
  virtual void populateTheLandmarkTableWidget(); ///< populates the table widget
  virtual ssc::Transform3D getTargetTransform() const;

  ImageLandmarksSourcePtr mFixedLandmarkSource;
  ImageLandmarksSourcePtr mMovingLandmarkSource;

  //gui
  RegistrationFixedImageStringDataAdapterPtr mFixedDataAdapter;
  RegistrationMovingImageStringDataAdapterPtr mMovingDataAdapter;

  QPushButton* mRegisterButton;

private:
  LandmarkImage2ImageRegistrationWidget(); ///< not implemented
};

/**
 * @}
 */
}//namespace cx

#endif /* CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_ */
