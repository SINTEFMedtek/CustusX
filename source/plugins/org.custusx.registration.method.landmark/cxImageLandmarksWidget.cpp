/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "cxViewGroupData.h"
#include "cxRepContainer.h"
#include "cxTrackingService.h"

namespace cx
{
ImageLandmarksWidget::ImageLandmarksWidget(RegServices services, QWidget* parent,
	QString objectName, QString windowTitle, bool useRegistrationFixedPropertyInsteadOfActiveImage) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle),
	mUseRegistrationFixedPropertyInsteadOfActiveImage(useRegistrationFixedPropertyInsteadOfActiveImage)
{
	if(mUseRegistrationFixedPropertyInsteadOfActiveImage)
		mCurrentProperty.reset(new StringPropertyRegistrationFixedImage(services.registrationService, services.patientModelService));
	else
		mCurrentProperty = StringPropertySelectData::New(mServices.patientModelService);
	connect(mCurrentProperty.get(), &Property::changed, this, &ImageLandmarksWidget::onCurrentImageChanged);
	mImageLandmarkSource = ImageLandmarksSource::New();

	mActiveToolProxy = ActiveToolProxy::New(services.trackingService);
	connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableButtons()));
	connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(enableButtons()));

	//pushbuttons
	mAddLandmarkButton = new QPushButton("New Landmark", this);
	mAddLandmarkButton->setToolTip("Add landmark");
	mAddLandmarkButton->setDisabled(true);
	connect(mAddLandmarkButton, SIGNAL(clicked()), this, SLOT(addLandmarkButtonClickedSlot()));

	mEditLandmarkButton = new QPushButton("Resample", this);
	mEditLandmarkButton->setToolTip("Resample existing landmark");
	mEditLandmarkButton->setDisabled(true);
	connect(mEditLandmarkButton, SIGNAL(clicked()), this, SLOT(editLandmarkButtonClickedSlot()));

	mRemoveLandmarkButton = new QPushButton("Clear", this);
	mRemoveLandmarkButton->setToolTip("Clear selected landmark");
	mRemoveLandmarkButton->setDisabled(true);
	connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

	//layout
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mCurrentProperty));
	mVerticalLayout->addWidget(mLandmarkTableWidget);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);

	QHBoxLayout* landmarkButtonsLayout = new QHBoxLayout;
	landmarkButtonsLayout->addWidget(mAddLandmarkButton);
	landmarkButtonsLayout->addWidget(mEditLandmarkButton);
	landmarkButtonsLayout->addWidget(mRemoveLandmarkButton);
	mVerticalLayout->addLayout(landmarkButtonsLayout);
}

ImageLandmarksWidget::~ImageLandmarksWidget()
{
}

void ImageLandmarksWidget::onCurrentImageChanged()
{
	DataPtr data = mCurrentProperty->getData();

	mImageLandmarkSource->setData(data);
	this->enableButtons();

	if (data && !mServices.registrationService->getFixedData())
		mServices.registrationService->setFixedData(data);

	this->setModified();
}

PickerRepPtr ImageLandmarksWidget::getPickerRep()
{
	return mServices.visualizationService->get3DReps(0, 0)->findFirst<PickerRep>();
//	return RepContainer(mServices.visualizationService->get3DView(0, 0))->findFirst<PickerRep>();

//	if (!mServices.visualizationService->get3DView(0, 0))
//		return PickerRepPtr();

//	return RepContainer::findFirstRep<PickerRep>(mServices.visualizationService->get3DView(0, 0)->getReps());
}

DataPtr ImageLandmarksWidget::getCurrentData() const
{
	return mImageLandmarkSource->getData();
}

void ImageLandmarksWidget::addLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Could not find a rep to add the landmark to.");
		return;
	}

	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	QString uid = mServices.patientModelService->addLandmark();
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
	image->getLandmarks()->setLandmark(Landmark(uid, pos_d));

    this->activateLandmark(uid);
}


void ImageLandmarksWidget::editLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Could not find a rep to edit the landmark for.");
		return;
	}

	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	QString uid = mActiveLandmark;
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
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
	mAddLandmarkButton->setEnabled(loaded);

	DataPtr image = this->getCurrentData();
	if (image)
	{
		mAddLandmarkButton->setToolTip(QString("Add landmark to image %1").arg(image->getName()));
		mEditLandmarkButton->setToolTip(QString("Resample landmark in image %1").arg(image->getName()));
	}
//	this->setModified();
}

void ImageLandmarksWidget::showEvent(QShowEvent* event)
{
	LandmarkRegistrationWidget::showEvent(event);

	if(!mUseRegistrationFixedPropertyInsteadOfActiveImage)
	{
		ImagePtr image = mServices.patientModelService->getActiveImage();
		if (image)
			mCurrentProperty->setValue(image->getUid());
	}

	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsIMAGE_REGISTRATED);

	LandmarkRepPtr rep = mServices.visualizationService->get3DReps(0, 0)->findFirst<LandmarkRep>();
	if (rep)
	{
		rep->setPrimarySource(mImageLandmarkSource);
		rep->setSecondarySource(LandmarksSourcePtr());
	}
}

void ImageLandmarksWidget::hideEvent(QHideEvent* event)
{
	LandmarkRegistrationWidget::hideEvent(event);

	if(mServices.visualizationService->get3DView(0, 0))
	{
		LandmarkRepPtr rep = mServices.visualizationService->get3DReps(0, 0)->findFirst<LandmarkRep>();
		if (rep)
		{
			rep->setPrimarySource(LandmarksSourcePtr());
			rep->setSecondarySource(LandmarksSourcePtr());
		}
	}
	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsNOT_REGISTRATED);
}

void ImageLandmarksWidget::prePaintEvent()
{
    LandmarkRegistrationWidget::prePaintEvent();

	std::vector<Landmark> landmarks = this->getAllLandmarks();

	//update buttons
	mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
	mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
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
