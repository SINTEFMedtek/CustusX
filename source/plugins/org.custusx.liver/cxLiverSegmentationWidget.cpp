/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverSegmentationWidget.h"

#include <vtkImageData.h>
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
#include "cxLiverVisibilityWidget.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxRegistrationTransform.h"
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

	this->removePreviousResults(runs);

	this->rebuildProgressBars(runs);
	mCurrentRunKey = "";
	mSegmentationGroup->setVisible(false);
	mProcessingInfoGroup->setVisible(true);

	mPreparedImageCache.clear();
	QList<QueuedRun> queue;
	foreach (const PlannedRun& run, runs)
	{
		QueuedRun queuedRun;
		queuedRun.iniFileName = run.iniFileName;
		queuedRun.key = run.key;

		if (this->needsPreparation(run.filter))
		{
			ImagePtr& prepared = mPreparedImageCache[run.image->getUid()];
			if (!prepared)
			{
				prepared = this->prepareImageForHeavyFilter(run.image);
				mServices->patient()->insertData(prepared);
			}
			queuedRun.image = prepared;
		}
		else
		{
			queuedRun.image = run.image;
		}
		queue << queuedRun;
	}
	mRunner->start(queue);
	this->updateRunButtonState();
}

ImagePtr LiverSegmentationWidget::prepareImageForHeavyFilter(ImagePtr image) const
{
	// One fixed name for whatever this ends up producing, rather than
	// chaining each step's own default suffix onto the previous result's
	// name (e.g. cropImage()'s own default would turn an already-cropped
	// "CT crop" into "CT crop crop", and a further resample into
	// "CT crop crop res" - compounding indefinitely across repeated runs
	// and confusing the resulting mesh names).
	QString preparedUid = image->getUid() + "_prepared";
	QString preparedName = image->getName() + " (prepared for segmentation)";

	// Logged at each step (this all runs synchronously on the main thread -
	// unlike contourFilter()'s marching-cubes/smoothing, none of this is
	// backgrounded, so a large-enough volume can freeze the GUI here too).
	// Kept deliberately so a future freeze is diagnosable from the log
	// instead of requiring guesswork about which step it's stuck in.
	int dims[3];
	image->getBaseVtkImageData()->GetDimensions(dims);
	CX_LOG_INFO() << "LiverSegmentationWidget: preparing " << image->getName()
	              << " (" << dims[0] << "x" << dims[1] << "x" << dims[2] << ") for a heavy filter...";

	// Auto-crop first: lossless (just removes surrounding air/background),
	// and often enough on its own. Skip it if it wouldn't actually shrink
	// anything (e.g. the volume was already cropped).
	CX_LOG_INFO() << "LiverSegmentationWidget: computing auto-crop box...";
	ImagePtr working = image;
	DoubleBoundingBox3D autoCropBox = computeAutoCropBox(image);
	if (!similar(autoCropBox, image->boundingBox()))
	{
		CX_LOG_INFO() << "LiverSegmentationWidget: cropping...";
		working = cropImage(mServices->patient(), image, autoCropBox, preparedUid, preparedName);
	}
	else
	{
		CX_LOG_INFO() << "LiverSegmentationWidget: auto-crop box matches the full volume, skipping crop.";
	}

	// Only resample (lossy - reduces resolution) if still too large after
	// cropping. Scale all three axes by total voxel count, not just x/y: a
	// whole-body scan's native in-plane resolution may already be <=512 (no
	// benefit from an in-plane-only cap), while its z extent is what
	// actually makes marching-cubes/smoothing slow.
	//
	// Empirically: a ~29M voxel mask (58MB, 16-bit) completes fast, a ~139M
	// voxel one (278MB) froze the main thread for 20+ minutes. Capped in
	// between the two, with some margin below the known-good side.
	const double maxVoxelCountForHeavySmoothing = 40000000;
	CX_LOG_INFO() << "LiverSegmentationWidget: resampling if still needed...";
	ImagePtr prepared = resampleImageToMaxVoxelCount(mServices->patient(), working, maxVoxelCountForHeavySmoothing, preparedUid, preparedName);
	int preparedDims[3];
	prepared->getBaseVtkImageData()->GetDimensions(preparedDims);
	CX_LOG_INFO() << "LiverSegmentationWidget: prepared " << image->getName() << " -> "
	              << preparedDims[0] << "x" << preparedDims[1] << "x" << preparedDims[2];
	return prepared;
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
	this->cleanupPreparedImages();
	this->updateRunButtonState();
}

void LiverSegmentationWidget::removePreviousResults(const QList<PlannedRun>& runs) const
{
	// Re-running a filter against the same source image previously just
	// added more meshes on top of whatever it had already produced, so
	// repeated runs in one session accumulated duplicates (most visibly for
	// Liver Vessels/Lesions, which run every time - Liver+Pancreas/Segments
	// happen to self-limit less often only because their heavier smoothing
	// makes people re-run them less). Remove each filter's own prior output
	// for that same source image first, so a re-run always leaves exactly
	// one current result per organ per source image.
	foreach (const PlannedRun& run, runs)
	{
		foreach (ORGAN_TYPE organType, organTypesFor(run.filter))
		{
			std::map<QString, MeshPtr> candidates = mServices->patient()->getDataOfType<Mesh>(organType);
			std::map<QString, MeshPtr>::iterator it;
			for (it = candidates.begin(); it != candidates.end(); ++it)
			{
				if (it->second && LiverVisibilityWidget::descendsFrom(mServices->patient(), it->second->getParentSpace(), run.image->getUid()))
					mServices->patient()->removeData(it->first);
			}
		}
	}
}

void LiverSegmentationWidget::cleanupPreparedImages()
{
	// The resampled copies created in runOrStopButtonClicked() are a purely
	// internal implementation detail of this run - remove them once it's
	// done (whether it finished or was stopped) so they don't linger as UI
	// clutter. Any mesh created from one is re-parented directly to the
	// original source image first, since that mesh's parent frame would
	// otherwise dangle once the copy it actually points to is gone.
	QMapIterator<QString, ImagePtr> i(mPreparedImageCache);
	while (i.hasNext())
	{
		i.next();
		QString originalUid = i.key();
		ImagePtr resampled = i.value();
		if (!resampled)
			continue;

		ImagePtr original = mServices->patient()->getData<Image>(originalUid);
		std::map<QString, MeshPtr> meshes = mServices->patient()->getDataOfType<Mesh>();
		std::map<QString, MeshPtr>::iterator m;
		for (m = meshes.begin(); m != meshes.end(); ++m)
		{
			if (!m->second || m->second->getParentSpace() != resampled->getUid())
				continue;
			if (original)
			{
				m->second->get_rMd_History()->setRegistration(original->get_rMd());
				m->second->get_rMd_History()->setParentSpace(original->getUid());
			}
		}

		mServices->patient()->removeData(resampled->getUid());
	}
	mPreparedImageCache.clear();
}

bool LiverSegmentationWidget::needsPreparation(FilterKind filter)
{
	// Smoothing in the contour step runs on the raw marching-cubes output, so
	// on a large/uncropped volume it can freeze the main thread for 20+
	// minutes with no way to stop it (the external process is already gone
	// by the time this runs in postProcess()). See prepareImageForHeavyFilter().
	// Scoped to fkLiverPancreas and fkLiverSegments only: both use the
	// heaviest smoothing level (GenericScriptFilter::
	// contourFilterSettingForOrganType() filtering=3, same as Fraxinus's own
	// Lungs/Heart/Lobes), and Segments additionally produces up to 8 meshes
	// per run - fkLiverVessels/fkLiverLesions use lighter smoothing
	// (filtering=1/2) and are left as-is, matching Fraxinus's own lung
	// filters running unprepared volumes of this size without issue.
	return filter == fkLiverPancreas || filter == fkLiverSegments;
}

QList<ORGAN_TYPE> LiverSegmentationWidget::organTypesFor(FilterKind filter)
{
	switch (filter)
	{
	case fkLiverPancreas: return QList<ORGAN_TYPE>() << otLIVER << otPANCREAS;
	case fkLiverVessels:  return QList<ORGAN_TYPE>() << otLIVER_VESSELS;
	case fkLiverLesions:  return QList<ORGAN_TYPE>() << otLIVER_LESIONS;
	case fkLiverSegments:
		return QList<ORGAN_TYPE>() << otLIVER_SEGMENT_1 << otLIVER_SEGMENT_2 << otLIVER_SEGMENT_3 << otLIVER_SEGMENT_4
		                            << otLIVER_SEGMENT_5 << otLIVER_SEGMENT_6 << otLIVER_SEGMENT_7 << otLIVER_SEGMENT_8;
	}
	return QList<ORGAN_TYPE>();
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
