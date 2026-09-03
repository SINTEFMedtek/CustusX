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
#include <QPair>

#include "cxLiverSegmentationRunner.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxImage.h"
#include "cxDefinitions.h"
#include "cxEnumConversion.h"
#include "cxSelectDataStringProperty.h"
#include "cxDataSelectWidget.h"
#include "cxImageAlgorithms.h"
#include "cxLogger.h"

namespace cx
{

LiverSegmentationWidget::LiverSegmentationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, this->getWidgetName(), "Liver Segmentation"),
	mServices(services),
	mRunner(new LiverSegmentationRunner(services, this)),
	mImageSelector(StringPropertyActiveImage::New(services->patient())),
	mImageSelector2(StringPropertySelectImage::New(services->patient()))
{
	connect(mRunner.get(), &LiverSegmentationRunner::filterStarted, this, &LiverSegmentationWidget::onFilterStarted);
	connect(mRunner.get(), &LiverSegmentationRunner::progressChanged, this, &LiverSegmentationWidget::onProgressChanged);
	connect(mRunner.get(), &LiverSegmentationRunner::allFinished, this, &LiverSegmentationWidget::onAllFinished);

	mImageSelector->setValueName("Volume 1 (Active)");
	mImageSelector->setHelp("The active CT or MR volume to segment");
	connect(mImageSelector.get(), &SelectDataStringPropertyBase::dataChanged, this, &LiverSegmentationWidget::updateRunButtonState);

	mImageSelector2->setValueName("Volume 2 (optional)");
	mImageSelector2->setHelp("An optional second CT or MR volume to run the same filters against");
	connect(mImageSelector2.get(), &SelectDataStringPropertyBase::dataChanged, this, &LiverSegmentationWidget::updateRunButtonState);

	mRunSegmentationButton = new QPushButton("Run Segmentation");
	mRunSegmentationButton->setEnabled(false);
	connect(mRunSegmentationButton, &QPushButton::clicked, this, &LiverSegmentationWidget::runOrStopButtonClicked);

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &LiverSegmentationWidget::updateRunButtonState);

	QGridLayout* imageSelectorLayout = new QGridLayout();
	new DataSelectWidget(mServices->view(), mServices->patient(), this, mImageSelector, imageSelectorLayout, 0);
	new DataSelectWidget(mServices->view(), mServices->patient(), this, mImageSelector2, imageSelectorLayout, 1);

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

QString LiverSegmentationWidget::filterLabel(FilterKind filter)
{
	switch (filter)
	{
	case fkLiverPancreas: return "Liver and Pancreas";
	case fkLiverVessels:  return "Liver Vessels";
	case fkLiverLesions:  return "Liver Lesions";
	case fkLiverSegments: return "Liver Segments (Couinaud)";
	}
	return "";
}

QString LiverSegmentationWidget::iniFileNameFor(FilterKind filter, IMAGE_MODALITY modality)
{
	if (modality == imCT)
	{
		switch (filter)
		{
		case fkLiverPancreas: return "python_LiverPancreas.ini";
		case fkLiverVessels:  return "python_LiverVessels.ini";
		case fkLiverLesions:  return "python_LiverLesions.ini";
		case fkLiverSegments: return "python_LiverSegments.ini";
		}
	}
	else if (modality == imMR)
	{
		switch (filter)
		{
		case fkLiverPancreas: return "python_LiverPancreas_MR.ini";
		case fkLiverVessels:  return ""; // no MR-capable TotalSegmentator model exists
		case fkLiverLesions:  return "python_LiverLesions_MR.ini";
		case fkLiverSegments: return "python_LiverSegments_MR.ini";
		}
	}
	return "";
}

QString LiverSegmentationWidget::progressLabel(const PlannedRun& run)
{
	return QString("%1 (%2)").arg(filterLabel(run.filter)).arg(run.image->getName());
}

QGroupBox* LiverSegmentationWidget::buildSegmentationGroup()
{
	mCheckBoxLiverPancreas = new QCheckBox(filterLabel(fkLiverPancreas));
	mCheckBoxLiverVessels = new QCheckBox(filterLabel(fkLiverVessels));
	mCheckBoxLiverLesions = new QCheckBox(filterLabel(fkLiverLesions));
	mCheckBoxLiverSegments = new QCheckBox(filterLabel(fkLiverSegments));
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

void LiverSegmentationWidget::rebuildProgressBars(const QList<PlannedRun>& runs)
{
	QLayoutItem* item;
	while ((item = mProgressBarsLayout->takeAt(0)) != nullptr)
	{
		delete item->widget();
		delete item;
	}
	mProgressBars.clear();

	foreach (const PlannedRun& run, runs)
	{
		QProgressBar* bar = new QProgressBar();
		bar->setRange(0, 100);
		bar->setValue(0);
		mProgressBarsLayout->addWidget(new QLabel(progressLabel(run)));
		mProgressBarsLayout->addWidget(bar);
		mProgressBars.insert(run.key, bar);
	}
}

void LiverSegmentationWidget::selectAll(bool checked)
{
	mCheckBoxLiverPancreas->setChecked(checked);
	mCheckBoxLiverVessels->setChecked(checked);
	mCheckBoxLiverLesions->setChecked(checked);
	mCheckBoxLiverSegments->setChecked(checked);
}

QList<LiverSegmentationWidget::PlannedRun> LiverSegmentationWidget::buildPlannedRuns() const
{
	QList<PlannedRun> runs;

	QList<ImagePtr> images;
	if (ImagePtr image1 = this->selectedImage1())
		images << image1;
	if (ImagePtr image2 = this->selectedImage2())
		images << image2;

	QList<QPair<FilterKind, QCheckBox*> > filters;
	filters << qMakePair(fkLiverPancreas, mCheckBoxLiverPancreas)
	        << qMakePair(fkLiverVessels, mCheckBoxLiverVessels)
	        << qMakePair(fkLiverLesions, mCheckBoxLiverLesions)
	        << qMakePair(fkLiverSegments, mCheckBoxLiverSegments);

	foreach (const ImagePtr& image, images)
	{
		for (int i = 0; i < filters.size(); ++i)
		{
			FilterKind filter = filters[i].first;
			QCheckBox* checkbox = filters[i].second;
			if (!checkbox->isChecked())
				continue;

			QString iniFileName = this->iniFileNameFor(filter, image->getModality());
			if (iniFileName.isEmpty())
			{
				reportWarning(QString("Liver segmentation: skipping %1 for %2 - no %3-capable model exists")
				              .arg(filterLabel(filter)).arg(image->getName()).arg(enum2string(image->getModality())));
				continue;
			}

			PlannedRun run;
			run.filter = filter;
			run.image = image;
			run.iniFileName = iniFileName;
			run.key = iniFileName + "@" + image->getUid();
			runs << run;
		}
	}
	return runs;
}

void LiverSegmentationWidget::runOrStopButtonClicked()
{
	if (mRunner->isRunning())
	{
		mRunner->stop();
		mRunSegmentationButton->setEnabled(false);
		return;
	}

	QList<PlannedRun> runs = this->buildPlannedRuns();
	if (runs.isEmpty())
		return;

	this->rebuildProgressBars(runs);
	mCurrentRunKey = "";
	mSegmentationGroup->setVisible(false);
	mProcessingInfoGroup->setVisible(true);

	QList<QueuedRun> queue;
	foreach (const PlannedRun& run, runs)
	{
		QueuedRun queuedRun;
		queuedRun.iniFileName = run.iniFileName;
		queuedRun.image = this->prepareInputForFilter(run.filter, run.image);
		queue << queuedRun;
	}
	mRunner->start(queue);
	this->updateRunButtonState();
}

void LiverSegmentationWidget::onFilterStarted(QString key)
{
	if (QProgressBar* finishedBar = mProgressBars.value(mCurrentRunKey, nullptr))
	{
		finishedBar->setRange(0, 100);
		finishedBar->setValue(100);
	}
	mCurrentRunKey = key;
}

void LiverSegmentationWidget::onProgressChanged(int percent)
{
	QProgressBar* bar = mProgressBars.value(mCurrentRunKey, nullptr);
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
	if (QProgressBar* finishedBar = mProgressBars.value(mCurrentRunKey, nullptr))
	{
		finishedBar->setRange(0, 100);
		finishedBar->setValue(100);
	}
	mCurrentRunKey = "";
	mProcessingInfoGroup->setVisible(false);
	mSegmentationGroup->setVisible(true);
	this->updateRunButtonState();
}

ImagePtr LiverSegmentationWidget::prepareInputForFilter(FilterKind filter, ImagePtr image) const
{
	if (filter != fkLiverPancreas)
		return image;

	// Smoothing in the contour step runs on the raw marching-cubes output, so
	// on a large/uncropped volume it can freeze the main thread for 20+
	// minutes (observed directly, for this filter, on a 278MB volume vs. a
	// 58MB one). Fraxinus avoids the same issue for lung segmentation by
	// capping the in-plane resolution before segmenting (see
	// resampleImageToMaxInPlaneResolution()); scoped to this filter only,
	// since it's the one the freeze was observed on and Fraxinus's own
	// lung filters run unresampled volumes of this size without issue.
	ImagePtr resampled = resampleImageToMaxInPlaneResolution(mServices->patient(), image, 512);
	mServices->patient()->insertData(resampled);
	return resampled;
}

ImagePtr LiverSegmentationWidget::selectedImage1() const
{
	return boost::dynamic_pointer_cast<Image>(mImageSelector->getData());
}

ImagePtr LiverSegmentationWidget::selectedImage2() const
{
	return mImageSelector2->getImage();
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

	mRunSegmentationButton->setText("Run Segmentation");
	bool hasImage = this->selectedImage1() != nullptr;
	mRunSegmentationButton->setEnabled(hasImage);
	mRunSegmentationButton->setToolTip(hasImage ? "" : "Select a CT or MR volume above to enable segmentation");
}

} /* namespace cx */
