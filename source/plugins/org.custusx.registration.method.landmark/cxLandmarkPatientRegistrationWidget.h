/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLANDMARKPATIENTREGISTRATIONWIDGET_H_
#define CXLANDMARKPATIENTREGISTRATIONWIDGET_H_

#include "cxLandmarkRegistrationWidget.h"
#include "cxRegistrationProperties.h"

#include "cxImage.h"
#include "cxTransform3D.h"

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


class LandmarkPatientRegistrationWidget: public LandmarkRegistrationWidget
{
Q_OBJECT

public:
	LandmarkPatientRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle);

protected slots:
	void registerSlot();
//	virtual void fixedDataChanged(); ///< listens to the datamanager for when the active image is changed

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
	virtual LandmarkMap getTargetLandmarks() const;
	virtual Transform3D getTargetTransform() const;
	virtual void setTargetLandmark(QString uid, Vector3D p_target);
	virtual QString getTargetName() const;
	virtual void performRegistration();

private:
	QPushButton* mRegisterButton;

	StringPropertyRegistrationFixedImagePtr mFixedProperty;
};


/**
 * @}
 */
}//namespace cx

#endif /* CXLANDMARKPATIENTREGISTRATIONWIDGET_H_ */
