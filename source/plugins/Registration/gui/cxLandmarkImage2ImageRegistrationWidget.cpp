/*
 * cxLandmarkImage2ImageRegistrationWidget.cpp
 *
 *  Created on: Sep 7, 2011
 *      Author: christiana
 */

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
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscProbeRep.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "cxViewManager.h"
#include "cxSettings.h"
#include "cxView3D.h"

namespace cx
{
LandmarkImage2ImageRegistrationWidget::LandmarkImage2ImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle) :
  LandmarkRegistrationWidget(regManager, parent, objectName, windowTitle)
{
	mLandmarkTableWidget->hide();

	mFixedDataAdapter.reset(new RegistrationFixedImageStringDataAdapter(regManager));
  mMovingDataAdapter.reset(new RegistrationMovingImageStringDataAdapter(regManager));
  mRegisterButton = new QPushButton("Register", this);
  mRegisterButton->setToolTip("Perform registration");
  connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mFixedDataAdapter));
  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mMovingDataAdapter));
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);

  QHBoxLayout* regLayout = new QHBoxLayout;
  regLayout->addWidget(mRegisterButton);
  mVerticalLayout->addLayout(regLayout);
  mVerticalLayout->addStretch();
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
  viewManager()->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);
}

void LandmarkImage2ImageRegistrationWidget::hideEvent(QHideEvent* event)
{
  LandmarkRegistrationWidget::hideEvent(event);
  viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
}

void LandmarkImage2ImageRegistrationWidget::populateTheLandmarkTableWidget()
{
}

ssc::LandmarkMap LandmarkImage2ImageRegistrationWidget::getTargetLandmarks() const
{
	ssc::ImagePtr moving = boost::shared_dynamic_cast<ssc::Image>(mManager->getMovingData());

  if(moving)
    return moving->getLandmarks();
  else
    return ssc::LandmarkMap();
}

void LandmarkImage2ImageRegistrationWidget::performRegistration()
{
//	if (mCurrentImage)
//	{
//		//make sure the fixedData is set
//		if(!mManager->getFixedData())
//			mManager->setFixedData(mCurrentImage);
//
//		//make sure the movingData is set
//		if(!mManager->getMovingData())
//			mManager->setFixedData(mCurrentImage);
//	}

	mManager->doImageRegistration();
	this->updateAvarageAccuracyLabel();
}

/** Return transform from target space to reference space
 *
 */
ssc::Transform3D LandmarkImage2ImageRegistrationWidget::getTargetTransform() const
{
  if (!mManager->getMovingData())
    return ssc::Transform3D::Identity();
  return mManager->getMovingData()->get_rMd();
}

}//namespace cx

