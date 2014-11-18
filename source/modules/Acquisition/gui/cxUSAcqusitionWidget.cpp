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

#include "cxUSAcqusitionWidget.h"

#include <QtWidgets>

#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "cxTrackingService.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDoubleWidgets.h"
#include "cxTypeConversions.h"
#include "cxPatientData.h"
#include "cxSoundSpeedConversionWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolDataAdapters.h"
#include "cxDoubleDataAdapterTemporalCalibration.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxProbeConfigWidget.h"
#include "cxDisplayTimerWidget.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxStringDataAdapterXml.h"
#include "cxUsReconstructionService.h"
#include "cxHelperWidgets.h"


namespace cx
{

USAcqusitionWidget::USAcqusitionWidget(AcquisitionDataPtr pluginData, QWidget* parent) :
	RecordBaseWidget(pluginData, parent, settings()->value("Ultrasound/acquisitionName").toString())
{
	this->setObjectName("USAcqusitionWidget");
	this->setWindowTitle("US Acquisition");

	connect(mPluginData->getReconstructer().get(), &UsReconstructionService::reconstructAboutToStart, this, &USAcqusitionWidget::reconstructAboutToStartSlot);
	connect(mPluginData->getReconstructer().get(), &UsReconstructionService::reconstructStarted, this, &USAcqusitionWidget::reconstructStartedSlot);
	connect(mPluginData->getReconstructer().get(), &UsReconstructionService::reconstructFinished, this, &USAcqusitionWidget::reconstructFinishedSlot);

	mAcquisition.reset(new USAcquisition(mBase));
	connect(mAcquisition.get(), SIGNAL(acquisitionDataReady()), this, SLOT(acquisitionDataReadySlot()));

	connect(mBase.get(), SIGNAL(stateChanged()), this, SLOT(acquisitionStateChangedSlot()));
	connect(mBase.get(), SIGNAL(started()), this, SLOT(recordStarted()));
	connect(mBase.get(), SIGNAL(acquisitionStopped()), this, SLOT(recordStopped()), Qt::DirectConnection);
	connect(mBase.get(), SIGNAL(cancelled()), this, SLOT(recordCancelled()));
	connect(mAcquisition.get(), &USAcquisition::saveDataCompleted, mPluginData->getReconstructer().get(), &UsReconstructionService::newDataOnDisk);

	mRecordSessionWidget->setDescriptionVisibility(false);

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
	RecordBaseWidget::mLayout->addLayout(editsLayout);
	new LabeledComboBoxWidget(this, ActiveProbeConfigurationStringDataAdapter::New(), editsLayout, 0);
	sscCreateDataWidget(this, mPluginData->getReconstructer()->getParam("Preset"), editsLayout, 1);

	QAction* optionsAction = this->createAction(this,
	      QIcon(":/icons/open_icon_library/system-run-5.png"),
	      "Details", "Show Details",
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

QString USAcqusitionWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>US Acquisition.</h3>"
		"<p><i>Record and reconstruct US data.</i></br>"
		"</html>";
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

	SoundSpeedConverterWidget* soundSpeedWidget = new SoundSpeedConverterWidget(this);
	connect(trackingService().get(), SIGNAL(dominantToolChanged(const QString&)), soundSpeedWidget, SLOT(setToolSlot(const QString&)));

	ProbeConfigWidget* probeWidget = new ProbeConfigWidget(this);
	probeWidget->getActiveProbeConfigWidget()->setVisible(false);

	SpinBoxGroupWidget* temporalCalibrationWidget = new SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New());

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
		mPluginData->getReconstructer()->startReconstruction();
	}
}

void USAcqusitionWidget::acquisitionStateChangedSlot()
{
	Acquisition::STATE state = mBase->getState();

	switch (state)
	{
	case Acquisition::sRUNNING :
		break;
	case Acquisition::sNOT_RUNNING :
		break;
	case Acquisition::sPOST_PROCESSING :
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
	std::set<cx::TimedAlgorithmPtr> threads = mPluginData->getReconstructer()->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->attach(threads);
}

void USAcqusitionWidget::reconstructStartedSlot()
{
	mBase->startPostProcessing();
}

void USAcqusitionWidget::reconstructFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mPluginData->getReconstructer()->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->detach(threads);
	mBase->stopPostProcessing();
}

}//namespace cx
