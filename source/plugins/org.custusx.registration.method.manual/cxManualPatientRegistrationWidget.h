/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMANUALPATIENTREGISTRATIONWIDGET_H
#define CXMANUALPATIENTREGISTRATIONWIDGET_H

#include "cxManualRegistrationWidget.h"
#include "cxPatientModelService.h"

namespace cx
{

/** Direct setting of patient registration
 *
 * Manipulate the image matrix rMpr via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \ingroup org_custusx_registration_method_manual
 *  \date Feb 16, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */
class ManualPatientRegistrationWidget: public BaseWidget
{
Q_OBJECT
public:
ManualPatientRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName);
	 ~ManualPatientRegistrationWidget()
	{
	}
protected:
	void showEvent(QShowEvent *event);
	RegServicesPtr mServices;

private slots:
	void matrixWidgetChanged();
	void patientMatrixChanged();

private:
	QString getDescription();
	bool	isValid() const;
	Transform3D getMatrixFromBackend();
	void setMatrixFromWidget(Transform3D M);

	QVBoxLayout*		mVerticalLayout;
	QLabel*				mLabel;
	Transform3DWidget*	mMatrixWidget;
};
} //cx
#endif // CXMANUALPATIENTREGISTRATIONWIDGET_H
