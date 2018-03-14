/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * cxShadingWidget.cpp
 *
 *  \date Aug 20, 2010
 *      \author christiana
 */
#include "cxShadingWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include "cxImageTF3D.h"
#include "cxShadingParamsInterfaces.h"
#include "cxImagePropertiesWidget.h"
#include "cxActiveData.h"

namespace cx
{

ImageRenderPropertiesWidget::ImageRenderPropertiesWidget(QWidget* parent) :
		BaseWidget(parent, "ImagePropertiesWidget", "Image Properties"),
		mImage(ImagePtr())
{
	mInterpolationType = new QComboBox(this);
	mInterpolationType->insertItem(0, "Nearest");
	mInterpolationType->insertItem(1, "Linear");
	mInterpolationType->insertItem(2, "Cubic");
	mInterpolationType->setToolTip("Change VTK interpolation type");

	connect(mInterpolationType, SIGNAL(currentIndexChanged(int)), this, SLOT(interpolationTypeChanged(int)));

	QLabel* interpolationTypeLabel = new QLabel("Volume interpolation type", this);

	QHBoxLayout* layout =  new QHBoxLayout(this);
	layout->addWidget(interpolationTypeLabel);
	layout->addWidget(mInterpolationType);
}

void ImageRenderPropertiesWidget::interpolationTypeChanged(int index)
{
	if (mImage)
		mImage->setInterpolationType(index);
}

void ImageRenderPropertiesWidget::imageChanged(ImagePtr image)
{
	mImage = image;
	if (mImage)
		mInterpolationType->setCurrentIndex(mImage->getInterpolationType());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ShadingWidget::ShadingWidget(ActiveDataPtr activeData, QWidget* parent,  bool connectToActiveImage) :
	BaseWidget(parent, "shading_widget", "Shading"),
	mLayout(new QVBoxLayout(this)),
	mActiveData(activeData),
	mActiveImageProxy(ActiveImageProxyPtr()),
	mImage(ImagePtr()),
	mImagePropertiesWidget(ImageRenderPropertiesWidgetPtr())
{
  this->init(connectToActiveImage);
}

ShadingWidget::~ShadingWidget()
{}

void ShadingWidget::init(bool connectToActiveImage)
{
  mShadingCheckBox = new QCheckBox("Shading", this);
  this->setToolTip("Volume shading properties");

  connect(mShadingCheckBox, &QCheckBox::toggled, this, &ShadingWidget::shadingToggledSlot);

  QGridLayout* shadingLayput = new QGridLayout();
	shadingLayput->addWidget(mShadingCheckBox, 0,0);
  SliderGroupWidget* shadingAmbientWidget = new SliderGroupWidget(this, DoublePropertyBasePtr(new DoublePropertyShadingAmbient(mActiveData)), shadingLayput, 1);
  SliderGroupWidget* shadingDiffuseWidget = new SliderGroupWidget(this, DoublePropertyBasePtr(new DoublePropertyShadingDiffuse(mActiveData)), shadingLayput, 2);
  SliderGroupWidget* shadingSpecularWidget = new SliderGroupWidget(this, DoublePropertyBasePtr(new DoublePropertyShadingSpecular(mActiveData)), shadingLayput, 3);
  SliderGroupWidget* shadingSpecularPowerWidget = new SliderGroupWidget(this, DoublePropertyBasePtr(new DoublePropertyShadingSpecularPower(mActiveData)), shadingLayput, 4);

  shadingAmbientWidget->setEnabled(false);
  shadingDiffuseWidget->setEnabled(false);
  shadingSpecularWidget->setEnabled(false);
  shadingSpecularPowerWidget->setEnabled(false);

	if (connectToActiveImage)
	{
		mActiveImageProxy = ActiveImageProxy::New(mActiveData);
		connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &ShadingWidget::activeImageChangedSlot);
		connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &ShadingWidget::activeImageChangedSlot);
	}

	mImagePropertiesWidget.reset(new ImageRenderPropertiesWidget(NULL));
	shadingLayput->addWidget(mImagePropertiesWidget.get(), 5, 0, 1, 2);
	mLayout->addLayout(shadingLayput);
	mLayout->addStretch(1);
	this->activeImageChangedSlot();
}

void ShadingWidget::shadingToggledSlot(bool val)
{
	if (mImage)
		mImage->setShadingOn(val);
}

void ShadingWidget::activeImageChangedSlot()
{
	ImagePtr activeImage = mActiveData->getActive<Image>();
	this->imageChangedSlot(activeImage);
}

void ShadingWidget::imageChangedSlot(ImagePtr image)
{
	mImage = image;
	if (mImage)
	{
		mShadingCheckBox->setChecked(mImage->getShadingOn());
		mImagePropertiesWidget->imageChanged(image);
	}
}

}//namespace cx
