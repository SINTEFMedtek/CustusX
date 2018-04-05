/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFASTLANDMARKPATIENTREGISTRATIONWIDGET_H
#define CXFASTLANDMARKPATIENTREGISTRATIONWIDGET_H

#include "cxLandmarkRegistrationWidget.h"
#include "cxRegistrationProperties.h"

#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxLandmarkPatientRegistrationWidget.h"

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
typedef Transform3D Transform3D;

/**
 * \file
 * \addtogroup org_custusx_registration_method_landmark
 * @{
 */
class FastLandmarkPatientRegistrationWidget: public LandmarkPatientRegistrationWidget
{
public:
	FastLandmarkPatientRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle);

protected:
	virtual void performRegistration();
};


/**
 * @}
 */
}//namespace cx

#endif // CXFASTLANDMARKPATIENTREGISTRATIONWIDGET_H
