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

//#include "cxRegistrationBaseWidget.h"
#include "cxStringDataAdapter.h"
#include "cxTransform3DWidget.h"
#include "cxData.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"

class ctkPluginContext;

namespace cx
{

//typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

/**\brief Direct setting of image registration
 *
 * Manipulate the image matrix rMd via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \date Feb 16, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class ManualImageRegistrationWidget: public BaseWidget
{
Q_OBJECT
public:
	ManualImageRegistrationWidget(ctkPluginContext *pluginContext, QWidget* parent, QString objectName, QString windowTitle);
	virtual ~ManualImageRegistrationWidget()
	{
//		mRegistrationService.reset();
	}
	virtual QString defaultWhatsThis() const;

protected:
	virtual void showEvent(QShowEvent* event);
	DataPtr mConnectedMovingImage;
	RegistrationServicePtr mRegistrationService;

private slots:
	void matrixWidgetChanged();
	void imageMatrixChanged();
	void movingDataChanged();

private:
	virtual QString getDescription() = 0;
	virtual Transform3D getMatrixFromBackend() = 0;
	virtual void setMatrixFromWidget(Transform3D M) = 0;

	QVBoxLayout* mVerticalLayout;
	QLabel* mLabel;

//	StringDataAdapterPtr mFixedImage;
//	StringDataAdapterPtr mMovingImage;
	Transform3DWidget* mMatrixWidget;
};

/**\brief Direct setting of image registration
 *
 * The matrix is the fMm transform, i.e. from moving to fixed image
 *
 *  \date 2014-06-13
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class ManualImage2ImageRegistrationWidget : public ManualImageRegistrationWidget
{
public:
	ManualImage2ImageRegistrationWidget(ctkPluginContext *context, QWidget* parent) :
		ManualImageRegistrationWidget(context, parent,"ManualImage2ImageRegistrationWidget", "Manual Image to Image Registration") {}
	virtual QString getDescription();
	virtual Transform3D getMatrixFromBackend();
	virtual void setMatrixFromWidget(Transform3D M);

	bool isValid() const;

};

/**\brief Direct setting of image registration
 *
 * Manipulate the image matrix rMd via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \date Feb 16, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class ManualImageTransformRegistrationWidget : public ManualImageRegistrationWidget
{
public:
	ManualImageTransformRegistrationWidget(ctkPluginContext *context, QWidget* parent) :
		ManualImageRegistrationWidget(context, parent, "ManualImageTransformRegistrationWidget", "Manual Image Transform Registration") {}
	virtual QString getDescription();
	virtual Transform3D getMatrixFromBackend();
	virtual void setMatrixFromWidget(Transform3D M);
};

/**\brief Direct setting of patient registration
 *
 * Manipulate the image matrix rMpr via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \date Feb 16, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */
class ManualPatientRegistrationWidget: public BaseWidget
{
Q_OBJECT
public:
ManualPatientRegistrationWidget(ctkPluginContext *pluginContext, QWidget* parent);
	 ~ManualPatientRegistrationWidget()
	{
	}
	 QString defaultWhatsThis() const;
protected:
	void showEvent(QShowEvent *event);
	RegistrationServicePtr mRegistrationService;
	PatientModelServicePtr mPatientModelService;

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

} /* namespace cx */
#endif /* CXMANUALREGISTRATIONWIDGET_H_ */
