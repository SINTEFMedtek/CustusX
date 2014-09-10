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
#include "cxServiceTrackerListener.h"
#include "cxRegistrationService.h"

namespace cx
{

typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

/**\brief Direct setting of image registration
 *
 * Manipulate the image matrix rMd via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \date Feb 16, 2012
 *  \author christiana
 */
class ManualImageRegistrationWidget: public BaseWidget
{
Q_OBJECT
public:
	ManualImageRegistrationWidget(ctkPluginContext *context, QWidget* parent);
	virtual ~ManualImageRegistrationWidget()
	{
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

	void initServiceListener();
	void onServiceAdded(RegistrationService* service);
	void onServiceRemoved(RegistrationService *service);

	QVBoxLayout* mVerticalLayout;
	QLabel* mLabel;

	StringDataAdapterPtr mFixedImage;
	StringDataAdapterPtr mMovingImage;
	Transform3DWidget* mMatrixWidget;

	boost::shared_ptr<ServiceTrackerListener<RegistrationService> > mServiceListener;
	ctkPluginContext *mPluginContext;

};

/**\brief Direct setting of image registration
 *
 * The matrix is the fMm transform, i.e. from moving to fixed image
 *
 *  \date 2014-06-13
 *  \author christiana
 */
class ManualImage2ImageRegistrationWidget : public ManualImageRegistrationWidget
{
public:
	ManualImage2ImageRegistrationWidget(ctkPluginContext *context, QWidget* parent) :
		ManualImageRegistrationWidget(context, parent) {}
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
 *  \author christiana
 */
class ManualImageTransformRegistrationWidget : public ManualImageRegistrationWidget
{
public:
	ManualImageTransformRegistrationWidget(ctkPluginContext *context, QWidget* parent) :
		ManualImageRegistrationWidget(context, parent) {}
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
 *  \author christiana
 */
//class ManualPatientRegistrationWidget: public BaseWidget
//{
//Q_OBJECT
//public:
//ManualPatientRegistrationWidget(ctkPluginContext *context, QWidget* parent);
//	virtual ~ManualPatientRegistrationWidget()
//	{
//	}
//	virtual QString defaultWhatsThis() const;

//private slots:
//	void matrixWidgetChanged();
//	void patientMatrixChanged();

//private:
//	QVBoxLayout* mVerticalLayout;
//	Transform3DWidget* mMatrixWidget;
//	ctkPluginContext *mPluginContext;
//};

} /* namespace cx */
#endif /* CXMANUALREGISTRATIONWIDGET_H_ */
