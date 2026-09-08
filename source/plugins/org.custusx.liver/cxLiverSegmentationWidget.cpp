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
#include <QDoubleSpinBox>

#include "cxLiverSegmentationRunner.h"
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
	// Column 1 is the combobox itself (0=label, 2=show/remove buttons).
	imageSelectorLayout->setColumnStretch(1, 1);

	mAdvancedOptionsButton = new QCheckBox("Show advanced options");
	connect(mAdvancedOptionsButton, &QCheckBox::toggled, this, &LiverSegmentationWidget::showAdvancedOptions);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(imageSelectorLayout);
	layout->addWidget(this->buildSegmentationGroup());
	layout->addWidget(mAdvancedOptionsButton);
	layout->addWidget(this->buildAdvancedOptionsGroup());
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

QGroupBox* LiverSegmentationWidget::buildAdvancedOptionsGroup()
{
	mCheckBoxFastMode = new QCheckBox("Fast (lower resolution, less memory)");
	mCheckBoxFastMode->setToolTip("Uses TotalSegmentator's low-resolution model - faster and "
	                               "substantially lighter on memory, at the cost of coarser output. "
	                               "Recommended for whole-body volumes or memory-constrained machines.");

	mMemoryLimitSpinBox = new QDoubleSpinBox();
	mMemoryLimitSpinBox->setRange(0, 512);
	mMemoryLimitSpinBox->setSingleStep(1);
	mMemoryLimitSpinBox->setSuffix(" GB");
	mMemoryLimitSpinBox->setSpecialValueText("Automatic");
	mMemoryLimitSpinBox->setValue(0);
	mMemoryLimitSpinBox->setToolTip("Memory limit for the TotalSegmentator process. If it exceeds this, "
	                                 "it is stopped cleanly instead of risking the whole system running "
	                                 "out of memory. \"Automatic\" uses a fraction of the machine's own "
	                                 "total RAM.");

	QGridLayout* advancedLayout = new QGridLayout();
	int row = 0;
	advancedLayout->addWidget(mCheckBoxFastMode, row++, 0, 1, 2);
	advancedLayout->addWidget(new QLabel("TotalSegmentator memory limit"), row, 0);
	advancedLayout->addWidget(mMemoryLimitSpinBox, row++, 1);

	mAdvancedOptionsGroup = new QGroupBox("Advanced options");
	mAdvancedOptionsGroup->setLayout(advancedLayout);
	mAdvancedOptionsGroup->setVisible(false);
	return mAdvancedOptionsGroup;
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

void LiverSegmentationWidget::showAdvancedOptions(bool show)
{
	mAdvancedOptionsGroup->setVisible(show);
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
			run.fastMode = mCheckBoxFastMode->isChecked();
			run.memoryLimitGB = mMemoryLimitSpinBox->value();
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
		queuedRun.fastMode = run.fastMode;
		queuedRun.memoryLimitGB = run.memoryLimitGB;

		ImagePtr& prepared = mPreparedImageCache[run.image->getUid()];
		if (!prepared)
		{
			prepared = this->prepareImageForHeavyFilter(run.image);
			// prepareImageForHeavyFilter() returns the input image itself,
			// unchanged, if it was already small enough to need neither
			// cropping nor resampling - nothing to insert/hide in that case,
			// and cleanupPreparedImages() knows not to remove it either.
			if (prepared != run.image)
			{
				mServices->patient()->insertData(prepared);
				// Purely an internal implementation detail (see cleanupPreparedImages()) -
				// exclude it from volume selectors etc. while the run is in progress.
				mServices->patient()->makeAvailable(prepared->getUid(), false);
			}
		}
		queuedRun.image = prepared;
		queue << queuedRun;
	}
	mRunner->start(queue);
	this->updateRunButtonState();
}

ImagePtr LiverSegmentationWidget::prepareImageForHeavyFilter(ImagePtr image) const
{
	// One fixed uid for whatever this ends up producing, rather than
	// chaining each step's own default suffix onto the previous result's
	// uid/name (e.g. cropImage()'s own default would turn an already-cropped
	// "CT crop" into "CT crop crop", and a further resample into
	// "CT crop crop res" - compounding indefinitely across repeated runs).
	// The *name* is kept identical to the original's, not decorated: the
	// resulting mesh's own name is derived from this image's name
	// (GenericScriptFilter uses its filter input's name to name the output
	// mask, which in turn names the mesh), and that name persists on the
	// mesh long after this temporary image is deleted - a decorated name
	// here would permanently show up in every mesh name.
	QString preparedUid = image->getUid() + "_prepared";
	QString preparedName = image->getName();

	// Logged at each step (this all runs synchronously on the main thread -
	// unlike contourFilter()'s marching-cubes/smoothing, none of this is
	// backgrounded, so a large-enough volume can freeze the GUI here too).
	// Kept deliberately so a future freeze is diagnosable from the log
	// instead of requiring guesswork about which step it's stuck in.
	int dims[3];
	image->getBaseVtkImageData()->GetDimensions(dims);
	CX_LOG_INFO() << "LiverSegmentationWidget: preparing " << image->getName()
	              << " (" << dims[0] << "x" << dims[1] << "x" << dims[2] << ") for a heavy filter...";

	// Auto-crop is lossless; skip it if it wouldn't actually shrink anything.
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
	// between the two, with some margin below the known-good side. A
	// whole-body scan (much longer along z than a torso/abdomen scan) is
	// still slow and memory-heavy even at that cap - not just for our own
	// marching cubes, but for TotalSegmentator's own inference, which runs
	// against this same prepared copy - so it gets an additional, lower cap.
	const double maxVoxelCountForHeavySmoothing = 40000000;
	const double maxVoxelCountForWholeBodyScan = 20000000;
	const double wholeBodyZExtentThresholdMm = 700;
	double voxelCap = maxVoxelCountForHeavySmoothing;
	if (working->boundingBox().range()[2] > wholeBodyZExtentThresholdMm)
	{
		voxelCap = maxVoxelCountForWholeBodyScan;
		CX_LOG_INFO() << "LiverSegmentationWidget: z extent exceeds " << wholeBodyZExtentThresholdMm
		              << "mm (whole-body scan) - using a lower voxel cap of " << voxelCap;
	}
	CX_LOG_INFO() << "LiverSegmentationWidget: resampling if still needed...";
	ImagePtr prepared = resampleImageToMaxVoxelCount(mServices->patient(), working, voxelCap, preparedUid, preparedName);
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
	// Re-running a filter against the same source image would otherwise just
	// add more meshes on top of previous output. Remove each filter's own
	// prior output for that same source image first, so a re-run always
	// leaves exactly one current result per organ per source image.
	foreach (const PlannedRun& run, runs)
	{
		foreach (ORGAN_TYPE organType, organTypesFor(run.filter))
		{
			std::map<QString, MeshPtr> candidates = mServices->patient()->getDataOfType<Mesh>(organType);
			std::map<QString, MeshPtr>::iterator it;
			for (it = candidates.begin(); it != candidates.end(); ++it)
			{
				if (it->second && it->second->getParentSpace() == run.image->getUid())
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
		if (!i.value())
			continue;
		// Not actually a separate prepared copy (see runOrStopButtonClicked()) -
		// this IS the original source image, never remove it.
		if (i.value()->getUid() == i.key())
			continue;
		this->reparentMeshesFromPreparedCopy(i.key(), i.value());
		mServices->patient()->removeData(i.value()->getUid());
	}
	mPreparedImageCache.clear();
}

void LiverSegmentationWidget::reparentMeshesFromPreparedCopy(QString originalUid, ImagePtr resampled) const
{
	// Only setParentSpace() is touched here - never setRegistration(). The
	// mesh's rMd was set relative to the cropped/resampled copy, which
	// encodes the crop offset; overwriting it with the original (pre-crop)
	// image's rMd would discard that offset and move the mesh by however far
	// the crop shifted it. setParentSpace() alone is pure bookkeeping and does
	// not affect the mesh's current rendered position.
	ImagePtr original = mServices->patient()->getData<Image>(originalUid);
	if (!original)
		return;

	std::map<QString, MeshPtr> meshes = mServices->patient()->getDataOfType<Mesh>();
	std::map<QString, MeshPtr>::iterator m;
	for (m = meshes.begin(); m != meshes.end(); ++m)
		if (m->second && m->second->getParentSpace() == resampled->getUid())
			m->second->get_rMd_History()->setParentSpace(original->getUid());
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
