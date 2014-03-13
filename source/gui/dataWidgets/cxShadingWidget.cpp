// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
#include "sscDataManager.h"
#include "sscImageTF3D.h"

//#include "sscAbstractInterface.h"
#include "cxShadingParamsInterfaces.h"

#include "cxImagePropertiesWidget.h"

namespace cx
{

ShadingWidget::ShadingWidget(QWidget* parent) :
		BaseWidget(parent, "ShadingWidget", "Shading"),
  mLayout(new QVBoxLayout(this))
{
  this->init();
}

ShadingWidget::~ShadingWidget()
{}

void ShadingWidget::init()
{
  mShadingCheckBox = new QCheckBox("Shading", this);
//  mLayout->addWidget(mShadingCheckBox);
//  mShadingCheckBox->setEnabled(true);
//
  connect(mShadingCheckBox, SIGNAL(toggled(bool)), this, SLOT(shadingToggledSlot(bool)));

  QGridLayout* shadingLayput = new QGridLayout();
	shadingLayput->addWidget(mShadingCheckBox, 0,0);
  SliderGroupWidget* shadingAmbientWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingAmbient()), shadingLayput, 1);
  SliderGroupWidget* shadingDiffuseWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingDiffuse()), shadingLayput, 2);
  SliderGroupWidget* shadingSpecularWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecular()), shadingLayput, 3);
  SliderGroupWidget* shadingSpecularPowerWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecularPower()), shadingLayput, 4);

  shadingAmbientWidget->setEnabled(false);
  shadingDiffuseWidget->setEnabled(false);
  shadingSpecularWidget->setEnabled(false);
  shadingSpecularPowerWidget->setEnabled(false);

  mActiveImageProxy = ActiveImageProxy::New(dataService());
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

	ImagePropertiesWidget* imagePropertiesWidget = new ImagePropertiesWidget(NULL);
	shadingLayput->addWidget(imagePropertiesWidget, 5, 0, 1, 2);
  mLayout->addLayout(shadingLayput);
	mLayout->addStretch(1);

//  this->setLayout(mLayout);
}

void ShadingWidget::shadingToggledSlot(bool val)
{
  ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    image->setShadingOn(val);
  }
}

void ShadingWidget::activeImageChangedSlot()
{
  ImagePtr activeImage = dataManager()->getActiveImage();

  if (activeImage)
  {
    //std::cout << "shading updated to " << activeImage->getShadingOn() << std::endl;
    mShadingCheckBox->setChecked(activeImage->getShadingOn());
  }
}

QString ShadingWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Shading</h3>"
		"<p>"
		"Set volume shading properties."
		"</p>"
		"<p><i></i></p>"
		"</html>";
}

}//namespace cx
