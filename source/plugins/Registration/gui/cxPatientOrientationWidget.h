#ifndef CXPATIENTORIENTATIONWIDGET_H_
#define CXPATIENTORIENTATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include "cxDominantToolProxy.h"

class QPushButton;
class QCheckBox;

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**
 * \class PatientOrientationWidget
 *
 * \brief Widget for setting reference space to current tool orientation,
 * without changing absolute position of data.
 *
 * \date 24. sep. 2012
 * \\author Christian Askeland
 */

class PatientOrientationWidget : public RegistrationBaseWidget
{
  Q_OBJECT

public:
  PatientOrientationWidget(RegistrationManagerPtr regManager, QWidget* parent);
  ~PatientOrientationWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void enableToolSampleButtonSlot();
  void setPatientOrientationSlot();
//  void patientOrientationFrontFaceSlot();
//  void patientOrientationBackFaceSlot();

private:
  ssc::Transform3D get_tMtm() const;
  QPushButton* mPatientOrientationButton;
  QCheckBox* mInvertButton;

//  QToolButton* mPatientOrientationFrontFaceButton;
//  QToolButton* mPatientOrientationBackFaceButton;
//  QAction* mPatientOrientationFrontFaceAction;
//  QAction* mPatientOrientationBackFaceAction;

  DominantToolProxyPtr mDominantToolProxy;
};

/**
 * @}
 */
}
#endif /* CXPATIENTORIENTATIONWIDGET_H_ */
