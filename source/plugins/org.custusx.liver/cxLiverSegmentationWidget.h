/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLIVERSEGMENTATIONWIDGET_H_
#define CXLIVERSEGMENTATIONWIDGET_H_

#include "org_custusx_liver_Export.h"
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"
#include <QMap>
#include <QList>
#include <QString>

class QCheckBox;
class QGroupBox;
class QPushButton;
class QLabel;
class QProgressBar;
class QVBoxLayout;
class QDoubleSpinBox;

namespace cx
{

typedef boost::shared_ptr<class LiverSegmentationRunner> LiverSegmentationRunnerPtr;
typedef boost::shared_ptr<class StringPropertyActiveImage> StringPropertyActiveImagePtr;
typedef boost::shared_ptr<class StringPropertySelectImage> StringPropertySelectImagePtr;

/**
 * Widget for selecting and running one or more of the liver segmentation
 * filters (Liver+Pancreas, Liver Vessels, Liver Lesions, Liver Segments)
 * against up to two source volumes (CT and/or MR). Liver Vessels has no
 * MR-capable TotalSegmentator model, so it is silently skipped (with a
 * warning) for any MR volume.
 *
 * \ingroup org_custusx_liver
 */
class org_custusx_liver_EXPORT LiverSegmentationWidget : public BaseWidget
{
	Q_OBJECT
public:
	explicit LiverSegmentationWidget(VisServicesPtr services, QWidget* parent = 0);
	virtual ~LiverSegmentationWidget();

	static QString getWidgetName();

	enum FilterKind
	{
		fkLiverPancreas,
		fkLiverVessels,
		fkLiverLesions,
		fkLiverSegments
	};
	struct PlannedRun
	{
		FilterKind filter;
		ImagePtr image;
		QString iniFileName;
		QString key;
		bool fastMode = false;
		double memoryLimitGB = 0; // 0 = automatic (see _process_utils.py)
	};

	// Pure/stateless - public so they can be unit tested directly, without
	// needing a live VisServices/patient session.
	static QString filterLabel(FilterKind filter);
	static QString iniFileNameFor(FilterKind filter, IMAGE_MODALITY modality);
	static QString progressLabel(const PlannedRun& run);
	// Single source of truth for this filter -> organ-type mapping.
	static QList<ORGAN_TYPE> organTypesFor(FilterKind filter);

private slots:
	void runOrStopButtonClicked();
	void selectAll(bool checked);
	void showAdvancedOptions(bool show);
	void updateRunButtonState();
	void onFilterStarted(QString key);
	void onProgressChanged(int percent);
	void onAllFinished();

private:
	QGroupBox* buildSegmentationGroup();
	QGroupBox* buildAdvancedOptionsGroup();
	QGroupBox* buildProcessingInfoGroup();
	void rebuildProgressBars(const QList<PlannedRun>& runs);
	QList<PlannedRun> buildPlannedRuns() const;
	ImagePtr selectedImage1() const;
	ImagePtr selectedImage2() const;
	void cleanupPreparedImages();

protected:
	// Not pure/stateless (mutates the patient model) - protected, exercised
	// via a thin test subclass, rather than left untested for lack of access.
	void removePreviousResults(const QList<PlannedRun>& runs) const;
	// Does NOT re-register the meshes (see cleanupPreparedImages()).
	void reparentMeshesFromPreparedCopy(QString originalUid, ImagePtr resampled) const;
	// Returns image itself, unchanged, if it is already small enough to need
	// neither cropping nor resampling - see runOrStopButtonClicked().
	ImagePtr prepareImageForHeavyFilter(ImagePtr image) const;

private:
	VisServicesPtr mServices;
	LiverSegmentationRunnerPtr mRunner;
	StringPropertyActiveImagePtr mImageSelector;
	StringPropertySelectImagePtr mImageSelector2;

	QCheckBox* mCheckBoxLiverPancreas;
	QCheckBox* mCheckBoxLiverVessels;
	QCheckBox* mCheckBoxLiverLesions;
	QCheckBox* mCheckBoxLiverSegments;
	QCheckBox* mCheckBoxSelectAll;
	QCheckBox* mAdvancedOptionsButton;
	QCheckBox* mCheckBoxFastMode;
	QDoubleSpinBox* mMemoryLimitSpinBox;
	QPushButton* mRunSegmentationButton;
	QGroupBox* mSegmentationGroup;
	QGroupBox* mAdvancedOptionsGroup;
	QGroupBox* mProcessingInfoGroup;
	QVBoxLayout* mProgressBarsLayout;
	QMap<QString, QProgressBar*> mProgressBars;
	QString mCurrentRunKey;
	QMap<QString, ImagePtr> mPreparedImageCache; // original source image uid -> its cropped/resampled copy (or itself, if already small enough), for this run only
};

} /* namespace cx */

#endif /* CXLIVERSEGMENTATIONWIDGET_H_ */
