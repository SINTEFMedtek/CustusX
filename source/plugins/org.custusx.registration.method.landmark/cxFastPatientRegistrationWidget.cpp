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

#include "cxFastPatientRegistrationWidget.h"

#include <QTableWidget>
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxLogger.h"

namespace cx
{
FastPatientRegistrationWidget::FastPatientRegistrationWidget(RegServices services, QWidget* parent) :
		PatientLandMarksWidget(services, parent, "org_custusx_registration_method_fast_landmark_image_to_patient_patient_landmarks_widget", "Fast Patient Registration")
{
	mMouseClickSample = new QCheckBox("Sample with mouse clicks in 2D.", this);
	mMouseClickSample->setToolTip("Allow mouse clicks in 2D views to sample patient landmarks");
	connect(mMouseClickSample, &QCheckBox::stateChanged, this, &FastPatientRegistrationWidget::mouseClickSampleStateChanged);

	mVerticalLayout->addWidget(mMouseClickSample);
}

FastPatientRegistrationWidget::~FastPatientRegistrationWidget()
{}

void FastPatientRegistrationWidget::performRegistration()
{
	mServices.registrationService->doFastRegistration_Translation();
	this->updateAverageAccuracyLabel();
}

void FastPatientRegistrationWidget::mouseClickSampleStateChanged()
{
	if(mMouseClickSample->isChecked())
		connect(mServices.visualizationService.get(), &VisualizationService::pointSampled, this, &FastPatientRegistrationWidget::pointSampled);
	else
		disconnect(mServices.visualizationService.get(), &VisualizationService::pointSampled, this, &FastPatientRegistrationWidget::pointSampled);
}

QTableWidgetItem * FastPatientRegistrationWidget::getLandmarkTableItem()
{
	if(!mLandmarkTableWidget)
		return NULL;

	int row = mLandmarkTableWidget->currentRow();
	int column = mLandmarkTableWidget->currentColumn();

	if((row < 0) && (mLandmarkTableWidget->rowCount() >= 0))
		row = 0;
	if((column < 0) && (mLandmarkTableWidget->columnCount() >= 0))
		column = 0;

	QTableWidgetItem* item = mLandmarkTableWidget->item(row, column);

	return item;
}

void FastPatientRegistrationWidget::pointSampled(Vector3D p_r)
{
	QTableWidgetItem* item = getLandmarkTableItem();
	if(!item)
	{
		CX_LOG_WARNING() << "FastPatientRegistrationWidget::pointSampled() Cannot get item from mLandmarkTableWidget";
		return;
	}
	QString uid = item->data(Qt::UserRole).toString();

	Transform3D rMtarget = this->getTargetTransform();
	Vector3D p_target = rMtarget.inv().coord(p_r);

	this->setTargetLandmark(uid, p_target);
	this->performRegistration();
}

void FastPatientRegistrationWidget::showEvent(QShowEvent* event)
{
	mMouseClickSample->setChecked(true);
	PatientLandMarksWidget::showEvent(event);
}

void FastPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
	mMouseClickSample->setChecked(false);
	PatientLandMarksWidget::hideEvent(event);
}

}//namespace cx
