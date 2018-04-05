/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRecordSessionWidget.h"

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QEvent>
#include "cxTime.h"

#include "cxRecordSession.h"
#include "cxTrackingService.h"
#include "cxTypeConversions.h"
#include "cxAcquisitionService.h"

namespace cx
{
RecordSessionWidget::RecordSessionWidget(AcquisitionServicePtr base, QWidget* parent,
										 AcquisitionService::TYPES context, QString category) :
    BaseWidget(parent, "RecordSessionWidget", "Record Session"),
	mAcquisitionService(base),
    mInfoLabel(new QLabel("")),
    mStartStopButton(new QPushButton(QIcon(":/icons/open_icon_library/media-record-3.png"), "Start")),
    mCancelButton(new QPushButton(QIcon(":/icons/open_icon_library/process-stop-7.png"), "Cancel")),
	mContext(context),
	mCategory(category)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(mInfoLabel);

	QHBoxLayout* buttonLayout = new QHBoxLayout();

	int buttonheight = mStartStopButton->sizeHint().height()*2.5;
	mStartStopButton->setMinimumHeight(buttonheight);
	mCancelButton->setMinimumHeight(buttonheight);

	buttonLayout->addWidget(mStartStopButton);
	buttonLayout->addWidget(mCancelButton);
	layout->addLayout(buttonLayout);
    QString warningText;
    warningText = "<font color=red>Note! This widget must be visible during active recording</font><br>";
    QLabel* warningInfoLabel = new QLabel(warningText);
    layout->addWidget(warningInfoLabel);

	connect(mAcquisitionService.get(), &AcquisitionService::stateChanged, this, &RecordSessionWidget::recordStateChangedSlot);

	mStartStopButton->setCheckable(true);
	connect(mStartStopButton, &QPushButton::clicked, this, &RecordSessionWidget::startStopSlot);
	connect(mCancelButton, &QPushButton::clicked, this, &RecordSessionWidget::cancelSlot);

	this->recordStateChangedSlot();

	connect(mAcquisitionService.get(), &AcquisitionService::usReadinessChanged, this, &RecordSessionWidget::onReadinessChanged);
	this->onReadinessChanged();
}

RecordSessionWidget::~RecordSessionWidget()
{}

void RecordSessionWidget::onReadinessChanged()
{
	this->setEnabled(mAcquisitionService->isReady(mContext));
	mInfoLabel->setText(mAcquisitionService->getInfoText(mContext));
}

void RecordSessionWidget::recordStateChangedSlot()
{
	AcquisitionService::STATE state = mAcquisitionService->getState();

	mStartStopButton->blockSignals(true);

	switch (state)
	{
	case AcquisitionService::sRUNNING :
	    mStartStopButton->setChecked(true);
		mStartStopButton->setText("Stop");
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/media-playback-stop.png"));
	    mStartStopButton->setEnabled(true);
	    mCancelButton->setEnabled(true);
		break;
	case AcquisitionService::sNOT_RUNNING :
	    mStartStopButton->setChecked(false);
		mStartStopButton->setText("Start");
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/media-record-3.png"));
	    mStartStopButton->setEnabled(true);
		mCancelButton->setEnabled(false);
		break;
	case AcquisitionService::sPOST_PROCESSING :
	    mStartStopButton->setChecked(false);
		mStartStopButton->setText("Processing...");
	    mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/media-record-3.png"));
	    mStartStopButton->setEnabled(false);
	    mCancelButton->setEnabled(false);
		break;
	}

	mStartStopButton->blockSignals(false);
}

void RecordSessionWidget::startStopSlot(bool checked)
{
	RecordSessionPtr session = mAcquisitionService->getSession(mCurrentSession);

	if (mAcquisitionService->getState()==AcquisitionService::sRUNNING)
		mAcquisitionService->stopRecord();
	else
		mAcquisitionService->startRecord(mContext, mCategory, session);

//	mAcquisitionService->toggleRecord(mContext, mCategory);
}

void RecordSessionWidget::cancelSlot()
{
	mAcquisitionService->cancelRecord();
}

void RecordSessionWidget::setCurrentSession(QString uid)
{
	mCurrentSession = uid;
}

}
