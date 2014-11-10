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

#include "cxRecordSessionWidget.h"

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QEvent>
#include "cxTime.h"
#include "cxReporter.h"
#include "cxRecordSession.h"
#include "cxTrackingService.h"
#include "cxTypeConversions.h"

namespace cx
{
RecordSessionWidget::RecordSessionWidget(AcquisitionPtr base, QWidget* parent, QString defaultDescription) :
    BaseWidget(parent, "RecordSessionWidget", "Record Session"),
    mBase(base),
    mInfoLabel(new QLabel("")),
    mStartStopButton(new QPushButton(QIcon(":/icons/open_icon_library/media-record-3.png"), "Start")),
    mCancelButton(new QPushButton(QIcon(":/icons/open_icon_library/process-stop-7.png"), "Cancel")),
    mDescriptionLine(new QLineEdit(defaultDescription))
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	mDescriptionLabel = new QLabel("Description:");
	layout->addWidget(mInfoLabel);
	layout->addWidget(mDescriptionLabel);
	layout->addWidget(mDescriptionLine);

	QHBoxLayout* buttonLayout = new QHBoxLayout();

	int buttonheight = mStartStopButton->sizeHint().height()*2.5;
	mStartStopButton->setMinimumHeight(buttonheight);
	mCancelButton->setMinimumHeight(buttonheight);

	buttonLayout->addWidget(mStartStopButton);
	buttonLayout->addWidget(mCancelButton);
	layout->addLayout(buttonLayout);

	connect(mBase.get(), SIGNAL(stateChanged()), this, SLOT(recordStateChangedSlot()));
	connect(mBase.get(), SIGNAL(readinessChanged()), this, SLOT(readinessChangedSlot()));

	mStartStopButton->setCheckable(true);
	connect(mStartStopButton, SIGNAL(clicked(bool)), this, SLOT(startStopSlot(bool)));
	connect(mCancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelSlot()));

	this->recordStateChangedSlot();
	this->readinessChangedSlot();
}

QString RecordSessionWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Record session.</h3>"
    "<p>Lets you record a session of some kind.</p>"
    "<p><i></i></p>"
    "</html>";
}

void RecordSessionWidget::setReady(bool val, QString text)
{
  this->setEnabled(val);
  mInfoLabel->setText(text);
}

void RecordSessionWidget::setDescriptionVisibility(bool value)
{
    mDescriptionLine->setVisible(value);
    mDescriptionLabel->setVisible(value);
}

RecordSessionWidget::~RecordSessionWidget()
{}

void RecordSessionWidget::setDescription(QString text)
{
  mDescriptionLine->setText(text);
}

void RecordSessionWidget::readinessChangedSlot()
{
	this->setEnabled(mBase->isReady());
    mInfoLabel->setText(mBase->getInfoText());
}

void RecordSessionWidget::recordStateChangedSlot()
{
	Acquisition::STATE state = mBase->getState();

	mStartStopButton->blockSignals(true);

	switch (state)
	{
	case Acquisition::sRUNNING :
	    mStartStopButton->setChecked(true);
		mStartStopButton->setText("Stop");
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/media-playback-stop.png"));
	    mStartStopButton->setEnabled(true);
	    mCancelButton->setEnabled(true);
		break;
	case Acquisition::sNOT_RUNNING :
	    mStartStopButton->setChecked(false);
		mStartStopButton->setText("Start");
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/media-record-3.png"));
	    mStartStopButton->setEnabled(true);
		mCancelButton->setEnabled(false);
		break;
	case Acquisition::sPOST_PROCESSING :
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
	mBase->toggleRecord();
}

void RecordSessionWidget::cancelSlot()
{
	mBase->cancelRecord();
}

}
