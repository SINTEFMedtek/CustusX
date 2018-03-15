/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPATIENTORIENTATIONWIDGET_H_
#define CXPATIENTORIENTATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "cxTransform3D.h"
#include "cxActiveToolProxy.h"
#include "cxPatientModelService.h"

class QPushButton;
class QCheckBox;

namespace cx
{

/**
 * \brief Widget for setting reference space to current tool orientation,
 * without changing absolute position of data.
 *
 * \ingroup org_custusx_registration_method_manual
 * \date 24. sep. 2012
 * \author Christian Askeland
 */

class PatientOrientationWidget : public RegistrationBaseWidget
{
  Q_OBJECT

public:
	PatientOrientationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle);
  ~PatientOrientationWidget();

private slots:
  void enableToolSampleButtonSlot();
  void setPatientOrientationSlot();

  void globalConfigurationFileChangedSlot(QString key);
private:
  Transform3D get_tMtm() const;
  QPushButton* mPatientOrientationButton;
  QCheckBox* mInvertButton;

  ActiveToolProxyPtr mActiveToolProxy;
};

}
#endif /* CXPATIENTORIENTATIONWIDGET_H_ */
