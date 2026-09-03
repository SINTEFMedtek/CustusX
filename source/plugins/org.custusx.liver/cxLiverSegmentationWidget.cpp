/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverSegmentationWidget.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLayoutItem>

#include "cxLiverSegmentationRunner.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxActiveData.h"
#include "cxImage.h"
#include "cxDefinitions.h"
#include "cxSelectDataStringProperty.h"
#include "cxDataSelectWidget.h"

namespace cx
{

LiverSegmentationWidget::LiverSegmentationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, this->getWidgetName(), "Liver Segmentation"),
	mServices(services),
	mRunner(new LiverSegmentationRunner(services, this)),
	mImageSelector(StringPropertySelectImage::New(services->patient()))
{
	connect(mRunner.get(), &LiverSegmentationRunner::filterStarted, this, &LiverSegmentationWidget::onFilterStarted);
	connect(mRunner.get(), &LiverSegmentationRunner::progressChanged, this, &LiverSegmentationWidget::onProgressChanged);
	connect(mRunner.get(), &LiverSegmentationRunner::allFinished, this, &LiverSegmentationWidget::onAllFinished);

	mImageSelector->setValueName("CT Volume");
	mImageSelector->setHelp("Select the CT volume to segment");
	ImagePtr activeImage = mServices->patient()->getActiveData()->getActive<Image>();
	if (activeImage)
		mImageSelector->setValue(activeImage->getUid());
	connect(mImageSelector.get(), &SelectDataStringPropertyBase::dataChanged, this, &LiverSegmentationWidget::updateRunButtonState);

	mRunSegmentationButton = new QPushButton("Run Segmentation");
	mRunSegmentationButton->setEnabled(false);
	connect(mRunSegmentationButton, &QPushButton::clicked, this, &LiverSegmentationWidget::runOrStopButtonClicked);

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &LiverSegmentationWidget::updateRunButtonState);

	QGridLayout* imageSelectorLayout = new QGridLayout();
	new DataSelectWidget(mServices->view(), mServices->patient(), this, mImageSelector, imageSelectorLayout, 0);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(imageSelectorLayout);
	layout->addWidget(this->buildSegmentationGroup());
	layout->addWidget(this->buildProcessingInfoGroup());
	layout->addWidget(mRunSegmentationButton);
	layout->addStretch();
	this->setLayout(layout);

	this->updateRunButtonState();
}

LiverSegmentationWidget::~LiverSegmentationWidget()
{
}

QString LiverSegmentationWidget::getWidgetName()
{
	return "liver_segmentation_widget";
}

QString LiverSegmentationWidget::friendlyName(QString iniFileName)
{
	if (iniFileName == "python_LiverPancreas.ini")
		return "Liver and Pancreas";
	if (iniFileName == "python_LiverVessels.ini")
		return "Liver Vessels";
	if (iniFileName == "python_LiverLesions.ini")
		return "Liver Lesions";
	if (iniFileName == "python_LiverSegments.ini")
		return "Liver Segments (Couinaud)";
	return iniFileName;
}

QGroupBox* LiverSegmentationWidget::buildSegmentationGroup()
{
	mCheckBoxLiverPancreas = new QCheckBox(this->friendlyName("python_LiverPancreas.ini"));
	mCheckBoxLiverVessels = new QCheckBox(this->friendlyName("python_LiverVessels.ini"));
	mCheckBoxLiverLesions = new QCheckBox(this->friendlyName("python_LiverLesions.ini"));
	mCheckBoxLiverSegments = new QCheckBox(this->friendlyName("python_LiverSegments.ini"));
	mCheckBoxSelectAll = new QCheckBox("Select all");
	connect(mCheckBoxSelectAll, &QCheckBox::toggled, this, &LiverSegmentationWidget::selectAll);

	QGridLayout* segLayout = new QGridLayout();
	int row = 0;
	segLayout->addWidget(mCheckBoxSelectAll, row++, 0);
	QFrame* separator = new QFrame();
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Sunken);
	segLayout->addWidget(separator, row++, 0);
	segLayout->addWidget(mCheckBoxLiverPancreas, row++, 0);
	segLayout->addWidget(mCheckBoxLiverVessels, row++, 0);
	segLayout->addWidget(mCheckBoxLiverLesions, row++, 0);
	segLayout->addWidget(mCheckBoxLiverSegments, row++, 0);

	mSegmentationGroup = new QGroupBox("Segmentation");
	mSegmentationGroup->setLayout(segLayout);
	return mSegmentationGroup;
}

QGroupBox* LiverSegmentationWidget::buildProcessingInfoGroup()
{
	mProgressBarsLayout = new QVBoxLayout();

	mProcessingInfoGroup = new QGroupBox("Segmentation status");
	mProcessingInfoGroup->setLayout(mProgressBarsLayout);
	mProcessingInfoGroup->setVisible(false);
	return mProcessingInfoGroup;
}

void LiverSegmentationWidget::rebuildProgressBars(QStringList iniFileNames)
{
	QLayoutItem* item;
	while ((item = mProgressBarsLayout->takeAt(0)) != nullptr)
	{
		delete item->widget();
		delete item;
	}
	mProgressBars.clear();

	foreach (QString iniFileName, iniFileNames)
	{
		QProgressBar* bar = new QProgressBar();
		bar->setRange(0, 100);
		bar->setValue(0);
		mProgressBarsLayout->addWidget(new QLabel(this->friendlyName(iniFileName)));
		mProgressBarsLayout->addWidget(bar);
		mProgressBars.insert(iniFileName, bar);
	}
}

void LiverSegmentationWidget::selectAll(bool checked)
{
	mCheckBoxLiverPancreas->setChecked(checked);
	mCheckBoxLiverVessels->setChecked(checked);
	mCheckBoxLiverLesions->setChecked(checked);
	mCheckBoxLiverSegments->setChecked(checked);
}

void LiverSegmentationWidget::runOrStopButtonClicked()
{
	if (mRunner->isRunning())
	{
		mRunner->stop();
		mRunSegmentationButton->setEnabled(false);
		return;
	}

	QStringList iniFileNames;
	if (mCheckBoxLiverPancreas->isChecked())
		iniFileNames << "python_LiverPancreas.ini";
	if (mCheckBoxLiverVessels->isChecked())
		iniFileNames << "python_LiverVessels.ini";
	if (mCheckBoxLiverLesions->isChecked())
		iniFileNames << "python_LiverLesions.ini";
	if (mCheckBoxLiverSegments->isChecked())
		iniFileNames << "python_LiverSegments.ini";
	if (iniFileNames.isEmpty())
		return;

	ImagePtr image = this->selectedImage();
	if (!image)
		return;

	this->rebuildProgressBars(iniFileNames);
	mCurrentIniFileName = "";
	mSegmentationGroup->setVisible(false);
	mProcessingInfoGroup->setVisible(true);
	mRunner->start(iniFileNames, image);
	this->updateRunButtonState();
}

void LiverSegmentationWidget::onFilterStarted(QString iniFileName)
{
	if (QProgressBar* finishedBar = mProgressBars.value(mCurrentIniFileName, nullptr))
	{
		finishedBar->setRange(0, 100);
		finishedBar->setValue(100);
	}
	mCurrentIniFileName = iniFileName;
}

void LiverSegmentationWidget::onProgressChanged(int percent)
{
	QProgressBar* bar = mProgressBars.value(mCurrentIniFileName, nullptr);
	if (!bar)
		return;

	if (percent < 0)
	{
		bar->setRange(0, 0);
	}
	else
	{
		if (bar->maximum() == 0)
			bar->setRange(0, 100);
		bar->setValue(percent);
	}
}

void LiverSegmentationWidget::onAllFinished()
{
	if (QProgressBar* finishedBar = mProgressBars.value(mCurrentIniFileName, nullptr))
	{
		finishedBar->setRange(0, 100);
		finishedBar->setValue(100);
	}
	mCurrentIniFileName = "";
	mProcessingInfoGroup->setVisible(false);
	mSegmentationGroup->setVisible(true);
	this->updateRunButtonState();
}

ImagePtr LiverSegmentationWidget::selectedImage() const
{
	return mImageSelector->getImage();
}

bool LiverSegmentationWidget::selectedImageIsCT() const
{
	ImagePtr image = this->selectedImage();
	return image && (image->getModality() == imCT);
}

void LiverSegmentationWidget::updateRunButtonState()
{
	if (mRunner->isRunning())
	{
		mRunSegmentationButton->setText("Stop Segmentation");
		mRunSegmentationButton->setEnabled(true);
		mRunSegmentationButton->setToolTip("Stop the running segmentation");
		return;
	}

	bool isCT = this->selectedImageIsCT();
	mRunSegmentationButton->setText("Run Segmentation");
	mRunSegmentationButton->setEnabled(isCT);
	if (!this->selectedImage())
		mRunSegmentationButton->setToolTip("Select a CT volume above to enable segmentation");
	else if (!isCT)
		mRunSegmentationButton->setToolTip("Selected volume is not a CT image");
	else
		mRunSegmentationButton->setToolTip("");
}

} /* namespace cx */
