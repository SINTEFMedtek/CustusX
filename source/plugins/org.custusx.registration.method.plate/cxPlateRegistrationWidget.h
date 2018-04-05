/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLATEREGISTRATIONWIDGET_H_
#define CXPLATEREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"

class QPushButton;
class QLabel;

namespace cx
{

/**
 * \class PlateRegistrationWidget
 *
 * \brief Widget for performing registration between a (dyna-CT) volume and a
 * predefined aurora tool with ct markers.
 *
 * \ingroup org_custusx_registration_method_plate
 * \date 10. nov. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class PlateRegistrationWidget : public RegistrationBaseWidget
{
	Q_OBJECT
public:
	PlateRegistrationWidget(RegServicesPtr services, QWidget* parent);
	virtual ~PlateRegistrationWidget();

private slots:
	virtual void showEvent(QShowEvent* event);
	virtual void hideEvent(QHideEvent* event);
	void landmarkUpdatedSlot();
	void plateRegistrationSlot();
	void internalUpdate();

private:
	QPushButton* mPlateRegistrationButton;
	QLabel*      mReferenceToolInfoLabel;
};

}
#endif /* CXPLATEREGISTRATIONWIDGET_H_ */
