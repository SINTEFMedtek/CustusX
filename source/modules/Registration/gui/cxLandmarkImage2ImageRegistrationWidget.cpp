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
#include "cxLandmarkImage2ImageRegistrationWidget.h"

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
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "cxViewManager.h"
#include "cxSettings.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxLogicManager.h"

namespace cx
{
LandmarkImage2ImageRegistrationWidget::LandmarkImage2ImageRegistrationWidget(RegistrationManagerPtr regManager,
	QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(regManager, parent, objectName, windowTitle)
{
	mLandmarkTableWidget->hide();

	mFixedLandmarkSource = ImageLandmarksSource::New();
	mMovingLandmarkSource = ImageLandmarksSource::New();

	connect(mManager.get(), SIGNAL(fixedDataChanged(QString)), this, SLOT(updateRep()));
	connect(mManager.get(), SIGNAL(movingDataChanged(QString)), this, SLOT(updateRep()));

	mFixedDataAdapter.reset(new RegistrationFixedImageStringDataAdapter(logicManager()->getPluginContext()));
	mMovingDataAdapter.reset(new RegistrationMovingImageStringDataAdapter(logicManager()->getPluginContext()));

	mTranslationCheckBox = new QCheckBox("Translation only", this);
	mTranslationCheckBox->setChecked(settings()->value("registration/I2ILandmarkTranslation", false).toBool());
	connect(mTranslationCheckBox, SIGNAL(toggled(bool)), this, SLOT(translationCheckBoxChanged()));

	mRegisterButton = new QPushButton("Register", this);
	mRegisterButton->setToolTip("Perform registration");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mFixedDataAdapter));
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mMovingDataAdapter));
	mVerticalLayout->addWidget(mTranslationCheckBox);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);

	QHBoxLayout* regLayout = new QHBoxLayout;
	regLayout->addWidget(mRegisterButton);
	mVerticalLayout->addLayout(regLayout);
	mVerticalLayout->addStretch();
}

void LandmarkImage2ImageRegistrationWidget::translationCheckBoxChanged()
{
	settings()->setValue("registration/I2ILandmarkTranslation", mTranslationCheckBox->isChecked());
}

void LandmarkImage2ImageRegistrationWidget::updateRep()
{
	mFixedLandmarkSource->setData(mManager->getFixedData());
	mMovingLandmarkSource->setData(mManager->getMovingData());
}

void LandmarkImage2ImageRegistrationWidget::registerSlot()
{
	this->performRegistration();
}

LandmarkImage2ImageRegistrationWidget::~LandmarkImage2ImageRegistrationWidget()
{
}

QString LandmarkImage2ImageRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Landmark based image to image registration.</h3>"
		"<p>Register moving image to fixed image. </p>"
		"</html>";
}

void LandmarkImage2ImageRegistrationWidget::showEvent(QShowEvent* event)
{
	LandmarkRegistrationWidget::showEvent(event);
	viewManager()->setRegistrationMode(rsIMAGE_REGISTRATED);

	LandmarkRepPtr rep = RepManager::findFirstRep<LandmarkRep>(viewManager()->get3DView(0, 0)->getReps());
	if (rep)
	{
		rep->setPrimarySource(mFixedLandmarkSource);
		rep->setSecondarySource(mMovingLandmarkSource);
		rep->setSecondaryColor(QColor::fromRgbF(0, 0.9, 0.5));
	}
}

void LandmarkImage2ImageRegistrationWidget::hideEvent(QHideEvent* event)
{
	LandmarkRegistrationWidget::hideEvent(event);

	if(viewManager()->get3DView(0, 0))
	{
		LandmarkRepPtr rep = RepManager::findFirstRep<LandmarkRep>(viewManager()->get3DView(0, 0)->getReps());
		if (rep)
		{
			rep->setPrimarySource(LandmarksSourcePtr());
			rep->setSecondarySource(LandmarksSourcePtr());
		}
	}
	viewManager()->setRegistrationMode(rsNOT_REGISTRATED);
}

void LandmarkImage2ImageRegistrationWidget::prePaintEvent()
{
}

LandmarkMap LandmarkImage2ImageRegistrationWidget::getTargetLandmarks() const
{
	ImagePtr moving = boost::dynamic_pointer_cast<Image>(mManager->getMovingData());

	if (moving)
		return moving->getLandmarks()->getLandmarks();
	else
		return LandmarkMap();
}

void LandmarkImage2ImageRegistrationWidget::performRegistration()
{
	mManager->doImageRegistration(mTranslationCheckBox->isChecked());
	this->updateAvarageAccuracyLabel();
}

/** Return transform from target space to reference space
 *
 */
Transform3D LandmarkImage2ImageRegistrationWidget::getTargetTransform() const
{
	if (!mManager->getMovingData())
		return Transform3D::Identity();
	return mManager->getMovingData()->get_rMd();
}

void LandmarkImage2ImageRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	ImagePtr image = boost::dynamic_pointer_cast<Image>(mManager->getMovingData());
	if (!image)
		return;
	image->getLandmarks()->setLandmark(Landmark(uid, p_target));
}

QString LandmarkImage2ImageRegistrationWidget::getTargetName() const
{
	DataPtr image = mManager->getMovingData();
	if (!image)
		return "None";
	return image->getName();
}


}//namespace cx

