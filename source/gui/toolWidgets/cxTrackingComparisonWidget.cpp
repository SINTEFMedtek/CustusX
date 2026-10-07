/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingComparisonWidget.h"

#include <algorithm>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QSet>
#include <QDateTime>
#include <QDir>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QSpinBox>
#include "cxDataLocations.h"
#include "cxDefinitions.h"
#include "cxProfile.h"
#include "cxSettings.h"
#include "cxTrackingService.h"
#include "cxTrackingSystemService.h"

namespace cx
{

TrackingComparisonWidget::TrackingComparisonWidget(TrackingServicePtr trackingService, VideoServicePtr videoService, QWidget* parent) :
	BaseWidget(parent, "tracking_comparison_widget", "Compare Tracking Systems"),
	mComparison(trackingService, videoService),
	mToolConfigFile(new QComboBox()),
	mOpenIGTLinkCommand(new QLineEdit(settings()->value("TrackingTest/openIGTLinkCommand", "").toString())),
	mOpenIGTLinkArguments(new QLineEdit(settings()->value("TrackingTest/openIGTLinkArguments", "").toString())),
	mOpenIGTLinkApplyReference(new QCheckBox("Apply the reference to the OpenIGTLink tools")),
	mDuration(new QSpinBox()),
	mOutputFolder(new QLineEdit(settings()->value("TrackingTest/folder", QDir::homePath()).toString())),
	mStartButton(new QPushButton("Start comparison")),
	mLog(new QPlainTextEdit()),
	mResultTable(new QTableWidget())
{
	this->setToolTip("Track the same tools with several tracking systems in turn, and record the positions");
	mOpenIGTLinkCommand->setToolTip("OpenIGTLink tracking server application to start for the openigtlink run, in its own folder. Empty: start the server yourself.");
	mOpenIGTLinkArguments->setToolTip("e.g. --hostname=/dev/ttyUSB0 --tools=a.rom,b.rom");
	mOpenIGTLinkApplyReference->setToolTip("On for servers that send positions in tracker space, off if the server applies the reference");
	mOpenIGTLinkApplyReference->setChecked(settings()->value("TrackingTest/openIGTLinkApplyReference", true).toBool());
	mDuration->setRange(1, 7200);
	mDuration->setSuffix(" s");
	mDuration->setValue(settings()->value("TrackingTest/duration_s", 10).toInt());
	mLog->setReadOnly(true);
	mLog->setLineWrapMode(QPlainTextEdit::NoWrap);
	mResultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mResultTable->verticalHeader()->hide();
	mResultTable->setToolTip("Jitter: length of the standard deviation of the positions. From first: distance between the mean positions in this and the first system. All values in summary.txt");
	mResultTable->hide();

	QGridLayout* layout = new QGridLayout(this);
	addLine(layout, "Tool configuration", this->createToolConfigurationSelector());
	addLine(layout, "Tracking systems", this->createTrackingSystemSelector(trackingService));
	addLine(layout, "OpenIGTLink server", mOpenIGTLinkCommand);
	addLine(layout, "Server arguments", mOpenIGTLinkArguments);
	addLine(layout, "", mOpenIGTLinkApplyReference);
	addLine(layout, "Record each for", mDuration);
	addLine(layout, "Output folder", mOutputFolder);
	layout->addWidget(mStartButton, layout->rowCount(), 0, 1, 2);
	layout->addWidget(mLog, layout->rowCount(), 0, 1, 2);
	layout->addWidget(mResultTable, layout->rowCount(), 0, 1, 2);

	connect(mStartButton, &QPushButton::clicked, this, &TrackingComparisonWidget::startOrCancel);
	connect(&mComparison, &TrackingSystemComparison::progress, mLog, &QPlainTextEdit::appendPlainText);
	connect(&mComparison, &TrackingSystemComparison::finished, this, &TrackingComparisonWidget::onFinished);
}

TrackingComparisonWidget::~TrackingComparisonWidget()
{
	disconnect(&mComparison, nullptr, this, nullptr);
	disconnect(&mComparison, nullptr, mLog, nullptr);
	mComparison.cancel();
}

void TrackingComparisonWidget::addLine(QGridLayout* layout, QString label, QWidget* widget)
{
	int row = layout->rowCount();
	layout->addWidget(new QLabel(label), row, 0);
	layout->addWidget(widget, row, 1);
}

QString TrackingComparisonWidget::getTrackingSystemUid(QString implementation)
{
	QString retval;
	if (implementation == TRACKING_SYSTEM_IMPLEMENTATION_IGSTK)
	{
		retval = "org.custusx.core.tracking.system.igstk";
	}
	else if (implementation == TRACKING_SYSTEM_IMPLEMENTATION_NDI)
	{
		retval = "org.custusx.core.tracking.system.ndi";
	}
	else if (implementation == TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK)
	{
		retval = "org.custusx.core.openigtlink3";
	}
	return retval;
}

QWidget* TrackingComparisonWidget::createTrackingSystemSelector(TrackingServicePtr trackingService)
{
	QStringList available;
	for (TrackingSystemServicePtr system : trackingService->getTrackingSystems())
	{
		available << system->getUid();
	}
	QStringList implementations = {TRACKING_SYSTEM_IMPLEMENTATION_IGSTK, TRACKING_SYSTEM_IMPLEMENTATION_NDI, TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK};
	QStringList selected = settings()->value("TrackingTest/implementations", implementations).toStringList();
	QWidget* retval = new QWidget();
	QHBoxLayout* layout = new QHBoxLayout(retval);
	layout->setContentsMargins(0, 0, 0, 0);
	for (const QString& implementation : implementations)
	{
		bool isAvailable = available.contains(getTrackingSystemUid(implementation));
		QCheckBox* checkBox = new QCheckBox(implementation);
		checkBox->setEnabled(isAvailable);
		checkBox->setChecked(isAvailable && selected.contains(implementation));
		checkBox->setToolTip(isAvailable ? QString("Track with %1").arg(implementation) : QString("%1 is not built into this application").arg(implementation));
		mImplementations[implementation] = checkBox;
		layout->addWidget(checkBox);
	}
	layout->addStretch();
	return retval;
}

QWidget* TrackingComparisonWidget::createToolConfigurationSelector()
{
	mToolConfigFile->setToolTip("The tools to track. The tracking system in the configuration is replaced for each system that is compared.");
	QString current = profile()->getToolConfigFilePath();
	if (!current.isEmpty())
	{
		this->addToolConfiguration(current, QFileInfo(current).fileName() + " (current)");
	}
	for (const QString& path : findToolConfigurations(getToolConfigurationFolders()))
	{
		if (QFileInfo(path).fileName() != QFileInfo(current).fileName())
		{
			this->addToolConfiguration(path, QFileInfo(path).fileName());
		}
	}
	QPushButton* browseButton = new QPushButton("Browse...");
	connect(browseButton, &QPushButton::clicked, this, &TrackingComparisonWidget::browseToolConfiguration);
	QWidget* retval = new QWidget();
	QHBoxLayout* layout = new QHBoxLayout(retval);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(mToolConfigFile, 1);
	layout->addWidget(browseButton);
	return retval;
}

void TrackingComparisonWidget::addToolConfiguration(QString path, QString name)
{
	mToolConfigFile->addItem(name, path);
	mToolConfigFile->setItemData(mToolConfigFile->count() - 1, path, Qt::ToolTipRole);
}

void TrackingComparisonWidget::browseToolConfiguration()
{
	QString path = QFileDialog::getOpenFileName(this, "Tool configuration", mToolConfigFile->currentData().toString(), "Tool configurations (*.xml)");
	if (!path.isEmpty())
	{
		this->addToolConfiguration(path, QFileInfo(path).fileName());
		mToolConfigFile->setCurrentIndex(mToolConfigFile->count() - 1);
	}
}

QStringList TrackingComparisonWidget::getToolConfigurationFolders()
{
	QStringList retval = profile()->getApplicationToolConfigPaths();
	for (const QString& root : DataLocations::getRootConfigPaths())
	{
		retval << root + "/profiles/" + profile()->getUid() + "/tool";
		retval << root + "/profiles/Laboratory/tool";
	}
	return retval;
}

QStringList TrackingComparisonWidget::findToolConfigurations(QStringList folders)
{
	QStringList retval;
	QSet<QString> names;
	for (const QString& folder : folders)
	{
		for (const QFileInfo& file : QDir(folder).entryInfoList(QStringList() << "*.xml", QDir::Files, QDir::Name))
		{
			if (!names.contains(file.fileName()))
			{
				names.insert(file.fileName());
				retval << file.absoluteFilePath();
			}
		}
	}
	std::sort(retval.begin(), retval.end(), [](const QString& a, const QString& b) { return QFileInfo(a).fileName() < QFileInfo(b).fileName(); });
	return retval;
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
	comparisonSettings.toolConfigFile = mToolConfigFile->currentData().toString();
	comparisonSettings.outputFolder = mOutputFolder->text() + "/tracking_comparison_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
	comparisonSettings.recordDuration_ms = mDuration->value() * 1000;
	comparisonSettings.openIGTLinkApplyReference = mOpenIGTLinkApplyReference->isChecked();
	comparisonSettings.runs = createRuns(this->getSelectedImplementations(), mOpenIGTLinkCommand->text(), mOpenIGTLinkArguments->text());
	mLog->clear();
	mResultTable->hide();
	mStartButton->setText("Cancel");
	mComparison.start(comparisonSettings);
}

void TrackingComparisonWidget::onFinished(bool success)
{
	Q_UNUSED(success);
	mStartButton->setText("Start comparison");
	this->showResults();
}

void TrackingComparisonWidget::showResults()
{
	std::vector<QStringList> rows = TrackingSystemComparison::createResultTable(mComparison.getResults());
	QStringList header = TrackingSystemComparison::getResultTableHeader();
	mResultTable->clear();
	mResultTable->setColumnCount(header.size());
	mResultTable->setHorizontalHeaderLabels(header);
	mResultTable->setRowCount(static_cast<int>(rows.size()));
	for (int row = 0; row < static_cast<int>(rows.size()); ++row)
	{
		for (int column = 0; column < rows[row].size(); ++column)
		{
			mResultTable->setItem(row, column, new QTableWidgetItem(rows[row][column]));
		}
	}
	mResultTable->resizeColumnsToContents();
	mResultTable->setVisible(!rows.empty());
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
