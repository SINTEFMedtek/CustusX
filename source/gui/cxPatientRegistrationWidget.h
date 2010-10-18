#ifndef CXPATIENTREGISTRATIONWIDGET_H_
#define CXPATIENTREGISTRATIONWIDGET_H_

#include "cxRegistrationWidget.h"

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

/**
 * \class PatientRegistrationWidget
 *
 * \brief Widget used as a tab in the ContexDockWidget for patient registration.
 *
 * \date Feb 3, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */
class PatientRegistrationWidget : public RegistrationWidget
{
  Q_OBJECT

public:
  PatientRegistrationWidget(QWidget* parent); ///< sets up layout and connects signals and slots
  virtual ~PatientRegistrationWidget(); ///< empty
  virtual QString defaultWhatsThis() const;

protected slots:

  virtual void activeImageChangedSlot(); ///< listens to the datamanager for when the active image is changed
  void toolVisibleSlot(bool visible); ///< enables/disables the Sample Tool button
  void toolSampleButtonClickedSlot(); ///< reacts when the Sample Tool button is clicked
  void dominantToolChangedSlot(const QString& uid); ///< set which tool to sample from
  void enableToolSampleButton();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  virtual void populateTheLandmarkTableWidget(ssc::ImagePtr image); ///< populates the table widget
  virtual ssc::LandmarkMap getTargetLandmarks() const;
  virtual ssc::Transform3D getTargetTransform() const;
  virtual void performRegistration();

  //gui
  QPushButton* mToolSampleButton; ///< the Sample Tool button

  //data
  ssc::ToolPtr mToolToSample; ///< tool to be sampled from

private:
  PatientRegistrationWidget(); ///< not implemented

};
}//namespace cx

#endif /* CXPATIENTREGISTRATIONWIDGET_H_ */
