/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFASTPATIENTREGISTRATIONWIDGET_H_
#define CXFASTPATIENTREGISTRATIONWIDGET_H_

#include "cxPatientLandMarksWidget.h"
#include "org_custusx_registration_method_landmark_Export.h"
#include <QCheckBox>

class QTableWidgetItem;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_method_landmark
 * @{
 */

/**
 * \class FastPatientRegistrationWidget
 *
 * \brief Widget for performing a fast and aprox landmark based patient registration,
 * using only the matrix translation part.
 *
 * \date 27. sep. 2010
 * \\author Janne Beate Bakeng
 */
class org_custusx_registration_method_landmark_EXPORT FastPatientRegistrationWidget : public PatientLandMarksWidget
{
	Q_OBJECT
public:
  FastPatientRegistrationWidget(RegServicesPtr services, QWidget* parent);
  virtual ~FastPatientRegistrationWidget();

protected:
  virtual void showEvent(QShowEvent *event);
  virtual void hideEvent(QHideEvent *event);

  QCheckBox* mMouseClickSample;

protected slots:
  void mouseClickSampleStateChanged();
  virtual void pointSampled(Vector3D p_r);
private:
  virtual void performRegistration();
  QTableWidgetItem * getLandmarkTableItem();
};

/**
 * @}
 */
}
#endif /* CXFASTPATIENTREGISTRATIONWIDGET_H_ */
