/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolPositionRecorderWidget.h"

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include "cxSettings.h"

namespace cx
{

ToolPositionRecorderWidget::ToolPositionRecorderWidget(TrackingServicePtr trackingService, QWidget* parent) :
	BaseWidget(parent, "tool_position_recorder_widget", "Record Positions"),
	mTrackingService(trackingService),
	mRecorder(trackingService),
	mFilename(new QLineEdit(createDefaultFilename())),
	mLabel(new QLineEdit(settings()->value("TrackingTest/recordLabel", "test").toString())),
	mRecordButton(new QPushButton("Start recording")),
	mStatus(new QLabel("Not recording")),
	mStatusTimer(new QTimer(this))
{
	this->setToolTip("Record tool positions, visibility and position warnings to a CSV file");
	mLabel->setToolTip("Written in the first column of each row, e.g. the tracking system or the test");
	mStatus->setWordWrap(true);
	mStatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
	QPushButton* browseButton = new QPushButton("Browse...");

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(new QLabel("File"), 0, 0);
	layout->addWidget(mFilename, 0, 1);
	layout->addWidget(browseButton, 0, 2);
	layout->addWidget(new QLabel("Label"), 1, 0);
	layout->addWidget(mLabel, 1, 1, 1, 2);
	layout->addWidget(mRecordButton, 2, 0, 1, 3);
	layout->addWidget(mStatus, 3, 0, 1, 3);
	layout->setRowStretch(4, 1);

	connect(browseButton, &QPushButton::clicked, this, &ToolPositionRecorderWidget::browse);
	connect(mRecordButton, &QPushButton::clicked, this, &ToolPositionRecorderWidget::toggleRecording);
	connect(mStatusTimer, &QTimer::timeout, this, &ToolPositionRecorderWidget::updateStatus);
}

ToolPositionRecorderWidget::~ToolPositionRecorderWidget()
{
}

QString ToolPositionRecorderWidget::createDefaultFilename()
{
	QString folder = settings()->value("TrackingTest/folder", QDir::homePath()).toString();
	return folder + "/tool_positions_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".csv";
}

void ToolPositionRecorderWidget::browse()
{
	QString filename = QFileDialog::getSaveFileName(this, "Record tool positions to", mFilename->text(), "CSV files (*.csv)");
	if (!filename.isEmpty())
	{
		mFilename->setText(filename);
	}
}

void ToolPositionRecorderWidget::toggleRecording()
{
	if (mRecorder.isRecording())
	{
		mRecorder.stop();
		mStatusTimer->stop();
		mFilename->setText(createDefaultFilename());
	}
	else if (mRecorder.start(mFilename->text(), mLabel->text()))
	{
		settings()->setValue("TrackingTest/folder", QFileInfo(mFilename->text()).absolutePath());
		settings()->setValue("TrackingTest/recordLabel", mLabel->text());
		mStatusTimer->start(500);
	}
	mRecordButton->setText(mRecorder.isRecording() ? "Stop recording" : "Start recording");
	this->updateStatus();
}

void ToolPositionRecorderWidget::updateStatus()
{
	QString state = mRecorder.isRecording() ? "Recording to " : "Recorded to ";
	QString text = mRecorder.getFilename().isEmpty() ? QString("Not recording")
													  : state + mRecorder.getFilename() + QString(", %1 rows").arg(mRecorder.getRowCount());
	QString summary = mRecorder.getSummary();
	mStatus->setText(summary.isEmpty() ? text : text + "\n\n" + summary);
}

} // namespace cx
