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
#include "cxRegistrationProperties.h"
#include "cxPC_ICP_Widget.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMeshHelpers.h"
#include "vtkPointData.h"

namespace cx
{
RMPCWidget::RMPCWidget(RegServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_pointcloud_widget",
						   "Point Cloud Registration"),
	mServices(services)
{
	mVerticalLayout = new QVBoxLayout(this);
	mOptions = profile()->getXmlSettings().descend("RMPCWidget");

//	mSurfaceSelector = StringPropertySelectMesh::New(mServices.patientModelService);
//	mSurfaceSelector->setValueName("Surface: ");

	mFixedImage.reset(new StringPropertyRegistrationFixedImage(services.registrationService, services.patientModelService));

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());

	mRecordTrackingWidget = new RecordTrackingWidget(mOptions.descend("recordTracker"),
													 mServices.acquisitionService,
													 mServices,
													 "tracker",
													 this);

//	mICPWidget = new PCICPWidget(mServices, this);
//	connect(mICPWidget, &PCICPWidget::registrationChanged, this, &RMPCWidget::onRegistration);
//	void setFixedData(DataPtr fixed);
//	void setMovingData(DataPtr moving);

	mVerticalLayout->setMargin(0);
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mFixedImage));
//	mVerticalLayout->addWidget(new DataSelectWidget(mServices.visualizationService, mServices.patientModelService, this, mSurfaceSelector));

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

//void RMPCWidget::onRegistration(Transform3D fMm)
//{
//	mServices.registrationService->applyPatientRegistration(new_rMpr, "Surface to Tracker");

//}

void RMPCWidget::registerSlot()
{
	QString logPath = mServices.patientModelService->getActivePatientFolder() + "/Logs/";

	DataPtr fixed = mServices.registrationService->getFixedData();
//	CX_LOG_CHANNEL_DEBUG("CA") << "trackerdata_r " << fixed->get->GetNumberOfCells();

	Transform3D rMpr = mServices.patientModelService->get_rMpr();
	TimedTransformMap trackerRecordedData_prMt = mRecordTrackingWidget->getRecordedTrackerData_prMt();
	CX_LOG_CHANNEL_DEBUG("CA") << "trackerRecordedData_prMt " << trackerRecordedData_prMt.size();
	vtkPolyDataPtr trackerdata_r = polydataFromTransforms(trackerRecordedData_prMt, rMpr);
	CX_LOG_CHANNEL_DEBUG("CA") << "trackerdata_r " << trackerdata_r->GetNumberOfCells();
	MeshPtr moving(new Mesh("tracker_temp"));
	moving->setVtkPolyData(trackerdata_r);
//	mServices.patientModelService->insertData(moving);

	SeansVesselReg vesselReg;
	vesselReg.mt_auto_lts = true;
	vesselReg.mt_ltsRatio = 80;
	vesselReg.mt_doOnlyLinear = true;
	vesselReg.mt_auto_lts = false;
	vesselReg.margin = 10E6;

	reportDebug("Using lts_ratio: " + qstring_cast(vesselReg.mt_ltsRatio));

	if(!moving)
	{
		reportWarning("Moving volume not set.");
		return;
	}
	else if(!fixed)
	{
		reportWarning("Fixed volume not set.");
		return;
	}

	bool success = vesselReg.execute(moving, fixed, logPath);
	if (!success)
	{
		reportWarning("ICP registration failed.");
		return;
	}

	Transform3D linearTransform = vesselReg.getLinearResult();
	std::cout << "v2v linear result:\n" << linearTransform << std::endl;
	//std::cout << "v2v inverted linear result:\n" << linearTransform.inverse() << std::endl;

	vesselReg.checkQuality(linearTransform);

	// The registration is performed in space r. Thus, given an old data position rMd, we find the
	// new one as rM'd = Q * rMd, where Q is the inverted registration output.
	// Delta is thus equal to Q:
	Transform3D delta = linearTransform.inv();
	std::cout << "delta:\n" << delta << std::endl;

	Transform3D new_rMpr = delta*rMpr;//output
	mServices.registrationService->applyPatientRegistration(new_rMpr, "Surface to Tracker");
}


} //namespace cx
