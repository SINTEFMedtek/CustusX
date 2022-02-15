/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImageLandmarksWidget.h"

#include <sstream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include "cxLogger.h"
#include "cxPickerRep.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxSettings.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxTypeConversions.h"
#include "cxSelectDataStringProperty.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxRepContainer.h"
#include "cxTrackingService.h"
#include "cxLandmarkListener.h"
#include "cxActiveData.h"
#include "cxPointMetric.h"
#include "cxSpaceProvider.h"

namespace cx
{
ImageLandmarksWidget::ImageLandmarksWidget(RegServicesPtr services, QWidget* parent,
										   QString objectName, QString windowTitle,
										   bool useRegistrationFixedPropertyInsteadOfActiveImage) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle),
	mUseRegistrationFixedPropertyInsteadOfActiveImage(useRegistrationFixedPropertyInsteadOfActiveImage),
	mLandmarksShowAdvancedSettingsString("Landmarks/ShowAdvanced")
{
	if(mUseRegistrationFixedPropertyInsteadOfActiveImage)
		mCurrentProperty.reset(new StringPropertyRegistrationFixedImage(services->registration(), services->patient()));
	else
		mCurrentProperty = StringPropertySelectData::New(mServices->patient());
	connect(mCurrentProperty.get(), &Property::changed, this, &ImageLandmarksWidget::onCurrentImageChanged);

	mLandmarkListener->useOnlyOneSourceUpdatedFromOutside();

	mActiveToolProxy = ActiveToolProxy::New(services->tracking());
	connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableButtons()));
	connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(enableButtons()));

	//pushbuttons
	mAddLandmarkButton = new QPushButton("New Landmark", this);
	mAddLandmarkButton->setToolTip("Add landmark");
	mAddLandmarkButton->setDisabled(true);
	connect(mAddLandmarkButton, SIGNAL(clicked()), this, SLOT(addLandmarkButtonClickedSlot()));

	mEditLandmarkButton = new QPushButton("Resample", this);
	mEditLandmarkButton->setToolTip("Resample the selected landmark");
	mEditLandmarkButton->setDisabled(true);
	connect(mEditLandmarkButton, SIGNAL(clicked()), this, SLOT(editLandmarkButtonClickedSlot()));

	mRemoveLandmarkButton = new QPushButton("Clear", this);
	mRemoveLandmarkButton->setToolTip("Clear the selected landmark");
	mRemoveLandmarkButton->setDisabled(true);
	connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

	mDeleteLandmarksButton = new QPushButton("Delete All", this);
	mDeleteLandmarksButton->setToolTip("Delete all landmarks");
	connect(mDeleteLandmarksButton, SIGNAL(clicked()), this, SLOT(deleteLandmarksButtonClickedSlot()));

	mImportLandmarksFromPointMetricsButton = new QPushButton("Import Point Metrics", this);
	mImportLandmarksFromPointMetricsButton->setToolTip("Import point metrics as landmarks. See the help pages for the details.");
	connect(mImportLandmarksFromPointMetricsButton, SIGNAL(clicked()), this, SLOT(importPointMetricsToLandmarkButtonClickedSlot()));

	//layout
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mCurrentProperty));
	mVerticalLayout->addWidget(mLandmarkTableWidget);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);

	QHBoxLayout* landmarkButtonsLayout = new QHBoxLayout;
	landmarkButtonsLayout->addWidget(mAddLandmarkButton);
	landmarkButtonsLayout->addWidget(mEditLandmarkButton);
	landmarkButtonsLayout->addWidget(mRemoveLandmarkButton);
	landmarkButtonsLayout->addWidget(mDeleteLandmarksButton);
	mDetailsAction = this->createAction(this,
										QIcon(":/icons/open_icon_library/system-run-5.png"),
										"Advanced", "Toggle advanced options",
										SLOT(toggleDetailsSlot()),
										landmarkButtonsLayout);
	mVerticalLayout->addLayout(landmarkButtonsLayout);

	QHBoxLayout* landmarkAdvancedButtonsLayout = new QHBoxLayout;
	landmarkAdvancedButtonsLayout = new QHBoxLayout;
	landmarkAdvancedButtonsLayout->addWidget(mImportLandmarksFromPointMetricsButton);
	mVerticalLayout->addLayout(landmarkAdvancedButtonsLayout);

	mMouseClickSample->show();
	connect(mMouseClickSample, &QCheckBox::stateChanged, this, &ImageLandmarksWidget::mouseClickSampleStateChanged);
	mVerticalLayout->addWidget(mMouseClickSample);

	this->showOrHideDetails();
}

ImageLandmarksWidget::~ImageLandmarksWidget()
{
}

void ImageLandmarksWidget::onCurrentImageChanged()
{
	DataPtr data = mCurrentProperty->getData();

	mLandmarkListener->setLandmarkSource(data);
	this->enableButtons();

	if (data && !mServices->registration()->getFixedData())
		mServices->registration()->setFixedData(data);

	this->setModified();
}

void ImageLandmarksWidget::toggleDetailsSlot()
{
	bool newShowAdvancedValue = !settings()->value(mLandmarksShowAdvancedSettingsString, "true").toBool();
	settings()->setValue(mLandmarksShowAdvancedSettingsString, newShowAdvancedValue);
	this->showOrHideDetails();
}

void ImageLandmarksWidget::showOrHideDetails()
{
	bool showAdvanced = settings()->value(mLandmarksShowAdvancedSettingsString).toBool();
	mImportLandmarksFromPointMetricsButton->setVisible(showAdvanced);
}

PickerRepPtr ImageLandmarksWidget::getPickerRep()
{
	return mServices->view()->get3DReps(0, 0)->findFirst<PickerRep>();
}

DataPtr ImageLandmarksWidget::getCurrentData() const
{
	return mLandmarkListener->getLandmarkSource();
}


void ImageLandmarksWidget::pointSampled(Vector3D p_r)
{
	this->resampleLandmark(p_r);
	//Only use the anyplane sampler for resample for now
	//this->addLandmark(p_r);
}

void ImageLandmarksWidget::addLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Need a 3D view to set landmarks.");
		return;
	}
	Vector3D pos_r = PickerRep->getPosition();
	this->addLandmark(pos_r);
}

void ImageLandmarksWidget::addLandmark(Vector3D p_r)
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;
	QString uid = mServices->patient()->addLandmark();
	Vector3D pos_d = image->get_rMd().inv().coord(p_r);
	image->getLandmarks()->setLandmark(Landmark(uid, pos_d));
	this->activateLandmark(uid);
}


void ImageLandmarksWidget::editLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Need a 3D view to edit landmarks.");
		return;
	}

	Vector3D pos_r = PickerRep->getPosition();
	this->resampleLandmark(pos_r);
}

void ImageLandmarksWidget::resampleLandmark(Vector3D p_r)
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;
	QString uid = mActiveLandmark;
	Vector3D pos_d = image->get_rMd().inv().coord(p_r);
	image->getLandmarks()->setLandmark(Landmark(uid, pos_d));

	this->activateLandmark(this->getNextLandmark());
}

void ImageLandmarksWidget::removeLandmarkButtonClickedSlot()
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	QString next = this->getNextLandmark();
	image->getLandmarks()->removeLandmark(mActiveLandmark);
	this->activateLandmark(next);
}

void ImageLandmarksWidget::deleteLandmarksButtonClickedSlot()
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	image->getLandmarks()->clear();
	this->setModified();
	mServices->patient()->deleteLandmarks();
}

void ImageLandmarksWidget::importPointMetricsToLandmarkButtonClickedSlot()
{
	DataPtr image = this->getCurrentData();
	if(!image)
		return;

	std::map<QString, DataPtr> point_metrics = mServices->patient()->getChildren(image->getUid(), PointMetric::getTypeName());
	std::map<QString, DataPtr>::iterator it = point_metrics.begin();

	//Make sure we have enough landmarks
	int number_of_landmarks = mServices->patient()->getLandmarkProperties().size();
	int number_of_metrics = point_metrics.size();
	for(int i=number_of_landmarks; i<number_of_metrics; ++i)
	{
		QString uid = mServices->patient()->addLandmark();
	}

	for(; it != point_metrics.end(); ++it)
	{
		PointMetricPtr point_metric = boost::static_pointer_cast<PointMetric>(it->second);
		if(!point_metric)
			continue;

		Vector3D pos_x = point_metric->getCoordinate();
		//Transform3D d_M_x = mServices->spaceProvider()->get_toMfrom(CoordinateSystem::fromString(image->getSpace()), point_metric->getSpace());
		//Vector3D pos_d = d_M_x.coord(pos_x);
		QString point_metric_name = point_metric->getName();
		image->getLandmarks()->setLandmark(Landmark(point_metric_name, pos_x));
		this->activateLandmark(point_metric_name);
	}
}

void ImageLandmarksWidget::cellClickedSlot(int row, int column)
{
	LandmarkRegistrationWidget::cellClickedSlot(row, column);
	this->enableButtons();
}

void ImageLandmarksWidget::enableButtons()
{
	bool selected = !mLandmarkTableWidget->selectedItems().isEmpty();
	bool loaded = this->getCurrentData() != 0;

	mEditLandmarkButton->setEnabled(selected);
	mRemoveLandmarkButton->setEnabled(selected);
	mDeleteLandmarksButton->setEnabled(loaded);
	mAddLandmarkButton->setEnabled(loaded);
	mImportLandmarksFromPointMetricsButton->setEnabled(loaded);

	DataPtr image = this->getCurrentData();
	if (image)
	{
		mAddLandmarkButton->setToolTip(QString("Add landmark to image %1").arg(image->getName()));
		mEditLandmarkButton->setToolTip(QString("Resample landmark in image %1").arg(image->getName()));
	}
}

void ImageLandmarksWidget::showEvent(QShowEvent* event)
{
	mServices->view()->setRegistrationMode(rsIMAGE_REGISTRATED);
	LandmarkRegistrationWidget::showEvent(event);

	if(!mUseRegistrationFixedPropertyInsteadOfActiveImage)
	{
		ActiveDataPtr activeData = mServices->patient()->getActiveData();
		ImagePtr image = activeData->getActive<Image>();
		if (image)
			mCurrentProperty->setValue(image->getUid());
	}
}

void ImageLandmarksWidget::hideEvent(QHideEvent* event)
{
	mServices->view()->setRegistrationMode(rsNOT_REGISTRATED);
	LandmarkRegistrationWidget::hideEvent(event);

}

void ImageLandmarksWidget::prePaintEvent()
{
	LandmarkRegistrationWidget::prePaintEvent();

	std::vector<Landmark> landmarks = this->getAllLandmarks();

	//update buttons
	mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
	mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
	this->showOrHideDetails();
}

LandmarkMap ImageLandmarksWidget::getTargetLandmarks() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return LandmarkMap();

	return image->getLandmarks()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D ImageLandmarksWidget::getTargetTransform() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return Transform3D::Identity();
	return image->get_rMd();
}

void ImageLandmarksWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;
	image->getLandmarks()->setLandmark(Landmark(uid, p_target));
}

QString ImageLandmarksWidget::getTargetName() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return "None";
	return image->getName();
}


}//namespace cx
