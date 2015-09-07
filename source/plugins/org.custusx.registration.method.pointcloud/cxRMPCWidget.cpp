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
#include "cxRMPCWidget.h"

#include <vtkPolyData.h>
#include "cxTransform3D.h"
#include "cxDataSelectWidget.h"
#include "cxTrackingService.h"
#include "cxMesh.h"
#include "cxSelectDataStringProperty.h"
#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxView.h"
#include "cxToolRep3D.h"
#include "cxToolTracer.h"
//#include "cxBronchoscopyRegistration.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxStringProperty.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTrackingService.h"
#include "cxDoubleProperty.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxBoolProperty.h"
#include "cxCheckBoxWidget.h"
#include "cxRepContainer.h"
#include "cxWidgetObscuredListener.h"
#include "cxViewGroupData.h"
#include "cxStringPropertySelectTool.h"
#include "cxHelperWidgets.h"
#include "cxAcquisitionService.h"
#include "cxRegServices.h"
#include "cxRecordTrackingWidget.h"
#include <QGroupBox>

namespace cx
{
RMPCWidget::RMPCWidget(RegServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_pointcloud_widget",
						   "Point Cloud Registration"),
	mServices(services)
{
	mVerticalLayout = new QVBoxLayout(this);
	mOptions = profile()->getXmlSettings().descend("RMPCWidget");

	mSurfaceSelector = StringPropertySelectMesh::New(mServices.patientModelService);
	mSurfaceSelector->setValueName("Surface: ");

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());

	mRecordTrackingWidget = new RecordTrackingWidget(mServices.acquisitionService, mServices, this);

	mVerticalLayout->setMargin(0);
	mVerticalLayout->addWidget(new DataSelectWidget(mServices.visualizationService, mServices.patientModelService, this, mSurfaceSelector));

	QVBoxLayout* trackLayout = this->createVBoxInGroupBox(mVerticalLayout, "Tracking Recorder");
	trackLayout->setMargin(0);
	trackLayout->addWidget(mRecordTrackingWidget);

	mVerticalLayout->addWidget(mRegisterButton);

	mVerticalLayout->addStretch();
}

QVBoxLayout* RMPCWidget::createVBoxInGroupBox(QVBoxLayout* parent, QString header)
{
	QWidget* widget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setMargin(0);

	QGroupBox* groupBox = this->wrapInGroupBox(widget, header);
	parent->addWidget(groupBox);

	return layout;
}

QString RMPCWidget::defaultWhatsThis() const
{
	return QString();
}

void RMPCWidget::registerSlot()
{
	std::cout << "NOT in USE" << std::endl;
//	if(!mBronchoscopyRegistration->isCenterlineProcessed())
//	{
//		reportError("Centerline not processed");
//		return;
//	}

	Transform3D old_rMpr = mServices.patientModelService->get_rMpr();//input to registrationAlgorithm
	//std::cout << "rMpr: " << std::endl;
	//std::cout << old_rMpr << std::endl;

	ToolPtr tool = mRecordTrackingWidget->getSuitableRecordingTool();

	TimedTransformMap trackerRecordedData_prMt = mRecordTrackingWidget->getRecordedTrackerData_prMt();

	if(trackerRecordedData_prMt.empty())
	{
		reportError("No positions");
		return;
	}

	Transform3D new_rMpr;
//	double maxDistanceForLocalRegistration = 30; //mm
//	if(mUseLocalRegistration->getValue())
//		new_rMpr = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistration(trackerRecordedData_prMt,old_rMpr,maxDistanceForLocalRegistration));
//	else
//		new_rMpr = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistration(trackerRecordedData_prMt,old_rMpr,0));

	new_rMpr = new_rMpr*old_rMpr;//output
	mServices.registrationService->applyPatientRegistration(new_rMpr, "Bronchoscopy centerline to tracking data");

	Eigen::Matrix4d display_rMpr = Eigen::Matrix4d::Identity();
			display_rMpr = new_rMpr*display_rMpr;
	std::cout << "New prMt: " << std::endl;
		for (int i = 0; i < 4; i++)
			std::cout << display_rMpr.row(i) << std::endl;

	mRecordTrackingWidget->ShowLastRecordingInView();
}


} //namespace cx
