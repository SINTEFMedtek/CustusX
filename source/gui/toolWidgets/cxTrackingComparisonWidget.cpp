/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingComparisonWidget.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include "cxDefinitions.h"
#include "cxProfile.h"
#include "cxSettings.h"

namespace cx
{

TrackingComparisonWidget::TrackingComparisonWidget(TrackingServicePtr trackingService, VideoServicePtr videoService, QWidget* parent) :
	BaseWidget(parent, "tracking_comparison_widget", "Compare Tracking Systems"),
	mComparison(trackingService, videoService),
	mToolConfigFile(new QLineEdit(profile()->getToolConfigFilePath())),
	mOpenIGTLinkCommand(new QLineEdit(settings()->value("TrackingTest/openIGTLinkCommand", "").toString())),
	mOpenIGTLinkArguments(new QLineEdit(settings()->value("TrackingTest/openIGTLinkArguments", "").toString())),
	mOpenIGTLinkApplyReference(new QCheckBox("Apply the reference to the OpenIGTLink tools")),
	mDuration(new QSpinBox()),
	mOutputFolder(new QLineEdit(settings()->value("TrackingTest/folder", QDir::homePath()).toString())),
	mStartButton(new QPushButton("Start comparison")),
	mLog(new QPlainTextEdit())
{
	this->setToolTip("Track the same tools with several tracking systems in turn, and record the positions");
	mOpenIGTLinkCommand->setToolTip("OpenIGTLink tracking server to start for the openigtlink run, e.g. bkinterface's ndistreamer");
	mOpenIGTLinkArguments->setToolTip("e.g. --hostname=/dev/ttyUSB0 --tools=a.rom,b.rom");
	mOpenIGTLinkApplyReference->setToolTip("On for servers that send positions in tracker space (e.g. bkinterface), off if the server applies the reference");
	mOpenIGTLinkApplyReference->setChecked(settings()->value("TrackingTest/openIGTLinkApplyReference", true).toBool());
	mDuration->setRange(1, 7200);
	mDuration->setSuffix(" s");
	mDuration->setValue(settings()->value("TrackingTest/duration_s", 10).toInt());
	mLog->setReadOnly(true);

	QHBoxLayout* systems = new QHBoxLayout();
	QStringList implementations = {TRACKING_SYSTEM_IMPLEMENTATION_IGSTK, TRACKING_SYSTEM_IMPLEMENTATION_NDI, TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK};
	QStringList selected = settings()->value("TrackingTest/implementations", implementations).toStringList();
	for (const QString& implementation : implementations)
	{
		mImplementations[implementation] = new QCheckBox(implementation);
		mImplementations[implementation]->setChecked(selected.contains(implementation));
		systems->addWidget(mImplementations[implementation]);
	}
	QWidget* systemsWidget = new QWidget();
	systemsWidget->setLayout(systems);

	QGridLayout* layout = new QGridLayout(this);
	addLine(layout, "Tool configuration", mToolConfigFile);
	addLine(layout, "Tracking systems", systemsWidget);
	addLine(layout, "OpenIGTLink server", mOpenIGTLinkCommand);
	addLine(layout, "Server arguments", mOpenIGTLinkArguments);
	addLine(layout, "", mOpenIGTLinkApplyReference);
	addLine(layout, "Record each for", mDuration);
	addLine(layout, "Output folder", mOutputFolder);
	layout->addWidget(mStartButton, layout->rowCount(), 0, 1, 2);
	layout->addWidget(mLog, layout->rowCount(), 0, 1, 2);

	connect(mStartButton, &QPushButton::clicked, this, &TrackingComparisonWidget::startOrCancel);
	connect(&mComparison, &TrackingSystemComparison::progress, mLog, &QPlainTextEdit::appendPlainText);
	connect(&mComparison, &TrackingSystemComparison::finished, this, &TrackingComparisonWidget::onFinished);
}

TrackingComparisonWidget::~TrackingComparisonWidget()
{
}

void TrackingComparisonWidget::addLine(QGridLayout* layout, QString label, QWidget* widget)
{
	int row = layout->rowCount();
	layout->addWidget(new QLabel(label), row, 0);
	layout->addWidget(widget, row, 1);
}

std::vector<TrackingComparisonRun> TrackingComparisonWidget::createRuns(QStringList implementations, QString openIGTLinkCommand, QString openIGTLinkArguments)
{
	std::vector<TrackingComparisonRun> retval;
	for (const QString& implementation : implementations)
	{
		TrackingComparisonRun run;
		run.implementation = implementation;
		if (implementation == TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK)
		{
			run.command = openIGTLinkCommand.trimmed();
			run.arguments = TrackingSystemComparison::splitArguments(openIGTLinkArguments);
		}
		retval.push_back(run);
	}
	return retval;
}

QStringList TrackingComparisonWidget::getSelectedImplementations() const
{
	QStringList retval;
	for (const std::pair<const QString, QCheckBox*>& entry : mImplementations)
	{
		if (entry.second->isChecked())
		{
			retval << entry.first;
		}
	}
	return retval;
}

void TrackingComparisonWidget::startOrCancel()
{
	if (mComparison.isRunning())
	{
		mComparison.cancel();
		return;
	}
	this->saveSettings();
	TrackingComparisonSettings comparisonSettings;
	comparisonSettings.toolConfigFile = mToolConfigFile->text();
	comparisonSettings.outputFolder = mOutputFolder->text() + "/tracking_comparison_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
	comparisonSettings.recordDuration_ms = mDuration->value() * 1000;
	comparisonSettings.openIGTLinkApplyReference = mOpenIGTLinkApplyReference->isChecked();
	comparisonSettings.runs = createRuns(this->getSelectedImplementations(), mOpenIGTLinkCommand->text(), mOpenIGTLinkArguments->text());
	mLog->clear();
	mStartButton->setText("Cancel");
	mComparison.start(comparisonSettings);
}

void TrackingComparisonWidget::onFinished(bool success)
{
	Q_UNUSED(success);
	mStartButton->setText("Start comparison");
	mLog->appendPlainText("\n" + mComparison.getSummary());
}

void TrackingComparisonWidget::saveSettings()
{
	settings()->setValue("TrackingTest/openIGTLinkCommand", mOpenIGTLinkCommand->text());
	settings()->setValue("TrackingTest/openIGTLinkArguments", mOpenIGTLinkArguments->text());
	settings()->setValue("TrackingTest/openIGTLinkApplyReference", mOpenIGTLinkApplyReference->isChecked());
	settings()->setValue("TrackingTest/duration_s", mDuration->value());
	settings()->setValue("TrackingTest/folder", mOutputFolder->text());
	settings()->setValue("TrackingTest/implementations", this->getSelectedImplementations());
}

} // namespace cx
