/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUSAcqusitionWidget.h"

#include <QtWidgets>

#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "cxTrackingService.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDoubleWidgets.h"
#include "cxTypeConversions.h"
#include "cxSoundSpeedConversionWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxDisplayTimerWidget.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxStringProperty.h"
#include "cxUsReconstructionService.h"
#include "cxHelperWidgets.h"
#include "cxVisServices.h"
#include "cxLogger.h"

#include "cxToolProperty.h"
#include "cxDoublePropertyTemporalCalibration.h"
#include "cxProbeConfigWidget.h"


namespace cx
{

USAcqusitionWidget::USAcqusitionWidget(AcquisitionServicePtr acquisitionService, VisServicesPtr services, UsReconstructionServicePtr usReconstructionService, QWidget* parent) :
	BaseWidget(parent, "org_custusx_acquisition_widgets_acquisition", "US Acquisition"),
	mUsReconstructionService(usReconstructionService),
	mAcquisitionService(acquisitionService),
	mLayout(new QVBoxLayout(this))
{
	this->setToolTip("Record and reconstruct US data");

	QString desc = settings()->value("Ultrasound/acquisitionName").toString();
	AcquisitionService::TYPES context(AcquisitionService::tUS);
	mRecordSessionWidget = new RecordSessionWidget(mAcquisitionService, this, context, desc);
	mLayout->addWidget(mRecordSessionWidget);

	mServices = services;
	connect(mUsReconstructionService.get(), &UsReconstructionService::reconstructAboutToStart, this, &USAcqusitionWidget::reconstructAboutToStartSlot);
	connect(mUsReconstructionService.get(), &UsReconstructionService::reconstructStarted, this, &USAcqusitionWidget::reconstructStartedSlot);
	connect(mUsReconstructionService.get(), &UsReconstructionService::reconstructFinished, this, &USAcqusitionWidget::reconstructFinishedSlot);

	connect(mAcquisitionService.get(), &AcquisitionService::stateChanged, this, &USAcqusitionWidget::acquisitionStateChangedSlot);
	connect(mAcquisitionService.get(), &AcquisitionService::started, this, &USAcqusitionWidget::recordStarted);
	connect(mAcquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &USAcqusitionWidget::recordStopped, Qt::DirectConnection);
	connect(mAcquisitionService.get(), &AcquisitionService::cancelled, this, &USAcqusitionWidget::recordCancelled);

	connect(mAcquisitionService.get(), &AcquisitionService::acquisitionDataReady, this, &USAcqusitionWidget::acquisitionDataReadySlot);
	connect(mAcquisitionService.get(), &AcquisitionService::saveDataCompleted, mUsReconstructionService.get(), &UsReconstructionService::newDataOnDisk);

	QHBoxLayout* timerLayout = new QHBoxLayout;
	mLayout->addLayout(timerLayout);
	mDisplayTimerWidget = new DisplayTimerWidget(this);
	mDisplayTimerWidget ->setFontSize(10);
	timerLayout->addStretch();
	timerLayout->addWidget(mDisplayTimerWidget);
	timerLayout->addStretch();

	QGridLayout* editsLayout = new QGridLayout;
	editsLayout->setColumnStretch(0,0);
	editsLayout->setColumnStretch(1,1);
	mLayout->addLayout(editsLayout);

	new LabeledComboBoxWidget(this, StringPropertyActiveProbeConfiguration::New(mServices->tracking()), editsLayout, 0);
	sscCreateDataWidget(this, mUsReconstructionService->getParam("Preset"), editsLayout, 1);

	QAction* optionsAction = this->createAction(this,
	      QIcon(":/icons/open_icon_library/system-run-5.png"),
          "Details", "Toggle Details",
	      SLOT(toggleDetailsSlot()),
	      NULL);

	QToolButton* optionsButton = new QToolButton();
	optionsButton->setDefaultAction(optionsAction);
	editsLayout->addWidget(optionsButton, 0, 2);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("acquisition/UsAcqShowDetails").toBool());

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mLayout->addWidget(mOptionsWidget);

	mLayout->addStretch();
	mLayout->addWidget(mTimedAlgorithmProgressBar);
}

USAcqusitionWidget::~USAcqusitionWidget()
{
}

void USAcqusitionWidget::toggleDetailsSlot()
{
  mOptionsWidget->setVisible(!mOptionsWidget->isVisible());
  settings()->setValue("acquisition/UsAcqShowDetails", mOptionsWidget->isVisible());
}

QWidget* USAcqusitionWidget::createOptionsWidget()
{
	QWidget* retval = new QWidget(this);
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);

	SoundSpeedConverterWidget* soundSpeedWidget = new SoundSpeedConverterWidget(mServices->tracking(), this);

	ProbeConfigWidget* probeWidget = new ProbeConfigWidget(mServices, this);
	probeWidget->getActiveProbeConfigWidget()->setVisible(false);

	SpinBoxGroupWidget* temporalCalibrationWidget = new SpinBoxGroupWidget(this, DoublePropertyTimeCalibration::New(mServices->tracking()));

	int line = 0;
	layout->addWidget(this->createHorizontalLine(), line++, 0, 1, 1);
	layout->addWidget(this->wrapGroupBox(probeWidget, "Probe", "Probe Definition"), line++, 0);
	layout->addWidget(this->wrapGroupBox(soundSpeedWidget, "Sound Speed", "Sound Speed"), line++, 0);
	layout->addWidget(temporalCalibrationWidget, line++, 0);

	return retval;
}

QWidget* USAcqusitionWidget::wrapGroupBox(QWidget* input, QString name, QString tip)
{
	QGroupBox* retval = new QGroupBox(name);
	retval->setToolTip(tip);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->addWidget(input);
	layout->addStretch();
	layout->setMargin(layout->margin()/2);
	return retval;
}

QWidget* USAcqusitionWidget::wrapVerticalStretch(QWidget* input)
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->addWidget(input);
	layout->addStretch();
	layout->setMargin(0);
	layout->setSpacing(0);
	return retval;
}

void USAcqusitionWidget::acquisitionDataReadySlot()
{
	if (settings()->value("Automation/autoReconstruct").toBool())
	{
		mUsReconstructionService->startReconstruction();
	}
}

void USAcqusitionWidget::acquisitionStateChangedSlot()
{
	AcquisitionService::STATE state = mAcquisitionService->getState();

	switch (state)
	{
	case AcquisitionService::sRUNNING :
		break;
	case AcquisitionService::sNOT_RUNNING :
		break;
	case AcquisitionService::sPOST_PROCESSING :
		break;
	}
}

void USAcqusitionWidget::recordStarted()
{
	mDisplayTimerWidget->start();
}
void USAcqusitionWidget::recordStopped()
{
	mDisplayTimerWidget->stop();
}
void USAcqusitionWidget::recordCancelled()
{
	mDisplayTimerWidget->stop();
}

void USAcqusitionWidget::reconstructAboutToStartSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mUsReconstructionService->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->attach(threads);
}

void USAcqusitionWidget::reconstructStartedSlot()
{
	mAcquisitionService->startPostProcessing();
}

void USAcqusitionWidget::reconstructFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mUsReconstructionService->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->detach(threads);
	mAcquisitionService->stopPostProcessing();
}

}//namespace cx
