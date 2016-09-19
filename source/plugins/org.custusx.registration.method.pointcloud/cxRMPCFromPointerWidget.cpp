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
#include "cxRMPCFromPointerWidget.h"

#include "cxMesh.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxStringProperty.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxWidgetObscuredListener.h"
#include "cxRecordTrackingWidget.h"
#include <QGroupBox>
#include "cxRegistrationProperties.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMeshHelpers.h"
#include "cxICPWidget.h"
#include "cxSpaceListener.h"
#include "cxSpaceProvider.h"
#include "cxAcquisitionService.h"
#include "cxRecordSessionSelector.h"
#include "cxLogger.h"
#include "cxTrackingService.h"

namespace cx
{
RMPCFromPointerWidget::RMPCFromPointerWidget(RegServicesPtr services, QWidget* parent) :
	ICPRegistrationBaseWidget(services, parent, "org_custusx_registration_method_pointcloud_frompointer_widget",
						   "Point Cloud Registration")
{
}

void RMPCFromPointerWidget::setup()
{
	mSpaceListenerMoving = mServices->spaceProvider()->createListener();
	mSpaceListenerFixed = mServices->spaceProvider()->createListener();
	mSpaceListenerMoving->setSpace(mServices->spaceProvider()->getPr());
	connect(mSpaceListenerMoving.get(), &SpaceListener::changed, this, &RMPCFromPointerWidget::onSpacesChanged);
	connect(mSpaceListenerFixed.get(), &SpaceListener::changed, this, &RMPCFromPointerWidget::onSpacesChanged);

	mFixedImage.reset(new StringPropertyRegistrationFixedImage(mServices->registration(), mServices->patient()));
	mFixedImage->setTypeRegexp("mesh");
	mFixedImage->setValueName("Select Surface");
	mFixedImage->setHelp("Select a surface model to register against.");

	connect(mServices->registration().get(), &RegistrationService::fixedDataChanged,
			this, &RMPCFromPointerWidget::inputChanged);

	mRecordTrackingWidget = new RecordTrackingWidget(mOptions.descend("recordTracker"),
													 mServices->acquisition(),
													 mServices,
													 "tracker",
													 this);
	mRecordTrackingWidget->displayToolSelector(false);
	connect(mRecordTrackingWidget->getSessionSelector().get(), &StringProperty::changed,
			this, &RMPCFromPointerWidget::inputChanged);
	this->connectAutoRegistration();

	connect(mServices->tracking().get(), &TrackingService::activeToolChanged, this, &RMPCFromPointerWidget::setModified);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new LabeledComboBoxWidget(this, mFixedImage));

	QVBoxLayout* trackLayout = this->createVBoxInGroupBox(layout, "Tracking Recorder");
	trackLayout->setMargin(0);
	trackLayout->addWidget(mRecordTrackingWidget);
	layout->addWidget(mICPWidget);
	layout->addStretch();

	this->inputChanged();
	this->onSettingsChanged();
}

void RMPCFromPointerWidget::connectAutoRegistration()
{
	// connect queued: we record based on the selected session and not the last one
	// and must thus wait for the selection to update
	connect(mRecordTrackingWidget, &RecordTrackingWidget::acquisitionCompleted, this,
			&RMPCFromPointerWidget::queuedAutoRegistration,
			Qt::QueuedConnection);
}


QVBoxLayout* RMPCFromPointerWidget::createVBoxInGroupBox(QVBoxLayout* parent, QString header)
{
	QWidget* widget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setMargin(0);

	QGroupBox* groupBox = this->wrapInGroupBox(widget, header);
	parent->addWidget(groupBox);

	return layout;
}

QString RMPCFromPointerWidget::defaultWhatsThis() const
{
	return QString();
}

void RMPCFromPointerWidget::initializeRegistrator()
{
	DataPtr fixed = mServices->registration()->getFixedData();
	MeshPtr moving = this->getTrackerDataAsMesh();
	QString logPath = mServices->patient()->getActivePatientFolder() + "/Logs/";

	mRegistrator->initialize(moving, fixed, logPath);
}

MeshPtr RMPCFromPointerWidget::getTrackerDataAsMesh()
{
	Transform3D rMpr = mServices->patient()->get_rMpr();

	TimedTransformMap trackerRecordedData_prMt = mRecordTrackingWidget->getSelectRecordSession()->getRecordedTrackerData_prMt();
	vtkPolyDataPtr trackerdata_r = polydataFromTransforms(trackerRecordedData_prMt, rMpr);

	MeshPtr moving(new Mesh("tracker_points"));
	moving->setVtkPolyData(trackerdata_r);
	return moving;
}

void RMPCFromPointerWidget::inputChanged()
{
	if (mObscuredListener->isObscured())
		return;

	DataPtr fixed = mServices->registration()->getFixedData();
	mSpaceListenerFixed->setSpace(mServices->spaceProvider()->getD(fixed));

	this->onSpacesChanged();
	this->setModified();
}

void RMPCFromPointerWidget::queuedAutoRegistration()
{
	if (!mObscuredListener->isObscured())
	{
		this->registerSlot();
	}
}

void RMPCFromPointerWidget::applyRegistration(Transform3D delta)
{
	ToolPtr tool = mRecordTrackingWidget->getSelectRecordSession()->getTool();
	Transform3D rMpr = mServices->patient()->get_rMpr();
	Transform3D new_rMpr = delta*rMpr;//output
	mServices->registration()->setLastRegistrationTime(QDateTime::currentDateTime());//Instead of restart
	QString text = QString("Contour from %1").arg(tool->getName());
	mServices->registration()->addPatientRegistration(new_rMpr, text);
}

void RMPCFromPointerWidget::onShown()
{
	this->inputChanged();
}



} //namespace cx
