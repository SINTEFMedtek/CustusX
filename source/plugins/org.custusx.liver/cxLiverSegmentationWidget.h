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

private slots:
	void runOrStopButtonClicked();
	void selectAll(bool checked);
	void updateRunButtonState();
	void onFilterStarted(QString key);
	void onProgressChanged(int percent);
	void onAllFinished();

private:
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
	};

	QGroupBox* buildSegmentationGroup();
	QGroupBox* buildProcessingInfoGroup();
	void rebuildProgressBars(const QList<PlannedRun>& runs);
	QList<PlannedRun> buildPlannedRuns() const;
	ImagePtr selectedImage1() const;
	ImagePtr selectedImage2() const;
	static bool needsResampling(FilterKind filter);
	static QString filterLabel(FilterKind filter);
	static QString iniFileNameFor(FilterKind filter, IMAGE_MODALITY modality);
	static QString progressLabel(const PlannedRun& run);

	VisServicesPtr mServices;
	LiverSegmentationRunnerPtr mRunner;
	StringPropertyActiveImagePtr mImageSelector;
	StringPropertySelectImagePtr mImageSelector2;

	QCheckBox* mCheckBoxLiverPancreas;
	QCheckBox* mCheckBoxLiverVessels;
	QCheckBox* mCheckBoxLiverLesions;
	QCheckBox* mCheckBoxLiverSegments;
	QCheckBox* mCheckBoxSelectAll;
	QPushButton* mRunSegmentationButton;
	QGroupBox* mSegmentationGroup;
	QGroupBox* mProcessingInfoGroup;
	QVBoxLayout* mProgressBarsLayout;
	QMap<QString, QProgressBar*> mProgressBars;
	QString mCurrentRunKey;
};

} /* namespace cx */

#endif /* CXLIVERSEGMENTATIONWIDGET_H_ */
