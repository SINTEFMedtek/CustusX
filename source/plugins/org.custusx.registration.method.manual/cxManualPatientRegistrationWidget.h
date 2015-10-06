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
