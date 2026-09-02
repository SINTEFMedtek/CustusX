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

class QCheckBox;
class QGroupBox;
class QPushButton;
class QLabel;

namespace cx
{

typedef boost::shared_ptr<class LiverSegmentationRunner> LiverSegmentationRunnerPtr;

/**
 * Widget for selecting and running one or more of the liver segmentation
 * filters (Liver+Pancreas, Liver Vessels, Liver Lesions, Liver Segments)
 * against the active CT image.
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
	void runSegmentationClicked();
	void selectAll(bool checked);
	void updateRunButtonState();
	void onFilterStarted(QString iniFileName);
	void onAllFinished();

private:
	QGroupBox* buildSegmentationGroup();
	QGroupBox* buildProcessingInfoGroup();
	bool activeImageIsCT() const;

	VisServicesPtr mServices;
	LiverSegmentationRunnerPtr mRunner;

	QCheckBox* mCheckBoxLiverPancreas;
	QCheckBox* mCheckBoxLiverVessels;
	QCheckBox* mCheckBoxLiverLesions;
	QCheckBox* mCheckBoxLiverSegments;
	QCheckBox* mCheckBoxSelectAll;
	QPushButton* mRunSegmentationButton;
	QGroupBox* mSegmentationGroup;
	QGroupBox* mProcessingInfoGroup;
	QLabel* mProcessingInfoLabel;
};

} /* namespace cx */

#endif /* CXLIVERSEGMENTATIONWIDGET_H_ */
