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
#include <QMap>
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
typedef boost::shared_ptr<class StringPropertySelectImage> StringPropertySelectImagePtr;

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
	void runOrStopButtonClicked();
	void selectAll(bool checked);
	void updateRunButtonState();
	void onFilterStarted(QString iniFileName);
	void onProgressChanged(int percent);
	void onAllFinished();

private:
	QGroupBox* buildSegmentationGroup();
	QGroupBox* buildProcessingInfoGroup();
	void rebuildProgressBars(QStringList iniFileNames);
	ImagePtr selectedImage() const;
	bool selectedImageIsCT() const;
	static QString friendlyName(QString iniFileName);

	VisServicesPtr mServices;
	LiverSegmentationRunnerPtr mRunner;
	StringPropertySelectImagePtr mImageSelector;

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
	QString mCurrentIniFileName;
};

} /* namespace cx */

#endif /* CXLIVERSEGMENTATIONWIDGET_H_ */
