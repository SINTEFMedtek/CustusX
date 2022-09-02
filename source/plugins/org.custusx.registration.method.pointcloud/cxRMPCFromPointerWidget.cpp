/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	mFixedImage->setTypeRegexp(Mesh::getTypeName());
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
