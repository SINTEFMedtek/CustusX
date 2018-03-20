/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMANUALREGISTRATIONWIDGET_H_
#define CXMANUALREGISTRATIONWIDGET_H_

#include "org_custusx_registration_method_manual_Export.h"

#include "cxBaseWidget.h"
#include "cxTransform3D.h"
#include "cxRegistrationMethodService.h"

class ctkPluginContext;

namespace cx
{
class Transform3DWidget;
typedef boost::shared_ptr<class Data> DataPtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;

//typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

/** Direct setting of image registration
 *
 * Manipulate the image matrix rMd via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \ingroup org_custusx_registration_method_manual
 *  \date Feb 16, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_manual_EXPORT ManualImageRegistrationWidget: public BaseWidget
{
Q_OBJECT
public:
	ManualImageRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle);
	virtual ~ManualImageRegistrationWidget() {}

protected:
	virtual void showEvent(QShowEvent* event);
	DataPtr mConnectedMovingImage;
	QVBoxLayout* mVerticalLayout;
	RegServicesPtr mServices;

private slots:
	void matrixWidgetChanged();
	void imageMatrixChanged();
	void movingDataChanged();

private:
	virtual QString getDescription() = 0;
	virtual Transform3D getMatrixFromBackend() = 0;
	virtual void setMatrixFromWidget(Transform3D M) = 0;

	QLabel* mLabel;

	Transform3DWidget* mMatrixWidget;
};

} /* namespace cx */
#endif /* CXMANUALREGISTRATIONWIDGET_H_ */
