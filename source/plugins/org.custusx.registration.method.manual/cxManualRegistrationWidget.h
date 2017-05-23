/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
