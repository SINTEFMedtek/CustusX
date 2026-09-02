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
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>

#include "cxLiverSegmentationRunner.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxActiveData.h"
#include "cxImage.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxDefinitions.h"

namespace cx
{

LiverSegmentationWidget::LiverSegmentationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, this->getWidgetName(), "Liver Segmentation"),
	mServices(services),
	mRunner(new LiverSegmentationRunner(services, this))
{
	connect(mRunner.get(), &LiverSegmentationRunner::filterStarted, this, &LiverSegmentationWidget::onFilterStarted);
	connect(mRunner.get(), &LiverSegmentationRunner::allFinished, this, &LiverSegmentationWidget::onAllFinished);

	mRunSegmentationButton = new QPushButton("Run Segmentation");
	mRunSegmentationButton->setEnabled(false);
	connect(mRunSegmentationButton, &QPushButton::clicked, this, &LiverSegmentationWidget::runSegmentationClicked);

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &LiverSegmentationWidget::updateRunButtonState);
	connect(mServices->patient()->getActiveData().get(), &ActiveData::activeImageChanged, this, &LiverSegmentationWidget::updateRunButtonState);

	QVBoxLayout* layout = new QVBoxLayout(this);
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

QGroupBox* LiverSegmentationWidget::buildSegmentationGroup()
{
	mCheckBoxLiverPancreas = new QCheckBox("Liver and Pancreas");
	mCheckBoxLiverVessels = new QCheckBox("Liver Vessels");
	mCheckBoxLiverLesions = new QCheckBox("Liver Lesions");
	mCheckBoxLiverSegments = new QCheckBox("Liver Segments (Couinaud)");
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
	mProcessingInfoLabel = new QLabel();
	QVBoxLayout* infoLayout = new QVBoxLayout();
	infoLayout->addWidget(mProcessingInfoLabel);
	infoLayout->addWidget(mRunner->getProgressBar());

	mProcessingInfoGroup = new QGroupBox("Segmentation status");
	mProcessingInfoGroup->setLayout(infoLayout);
	mProcessingInfoGroup->setVisible(false);
	return mProcessingInfoGroup;
}

void LiverSegmentationWidget::selectAll(bool checked)
{
	mCheckBoxLiverPancreas->setChecked(checked);
	mCheckBoxLiverVessels->setChecked(checked);
	mCheckBoxLiverLesions->setChecked(checked);
	mCheckBoxLiverSegments->setChecked(checked);
}

void LiverSegmentationWidget::runSegmentationClicked()
{
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

	ImagePtr image = mServices->patient()->getActiveData()->getActive<Image>();
	if (!image)
		return;

	mSegmentationGroup->setVisible(false);
	mProcessingInfoGroup->setVisible(true);
	mRunSegmentationButton->setEnabled(false);
	mRunner->start(iniFileNames, image);
}

void LiverSegmentationWidget::onFilterStarted(QString iniFileName)
{
	mProcessingInfoLabel->setText(QString("Running: %1").arg(iniFileName));
}

void LiverSegmentationWidget::onAllFinished()
{
	mProcessingInfoGroup->setVisible(false);
	mSegmentationGroup->setVisible(true);
	this->updateRunButtonState();
}

bool LiverSegmentationWidget::activeImageIsCT() const
{
	ImagePtr image = mServices->patient()->getActiveData()->getActive<Image>();
	return image && (image->getModality() == imCT);
}

void LiverSegmentationWidget::updateRunButtonState()
{
	mRunSegmentationButton->setEnabled(this->activeImageIsCT() && !mRunner->isRunning());
}

} /* namespace cx */
