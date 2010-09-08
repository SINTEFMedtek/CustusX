/*
 * cxShadingWidget.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */
#include "cxShadingWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include "sscDataManager.h"
#include "sscImageTF3D.h"

//#include "sscAbstractInterface.h"
#include "cxShadingParamsInterfaces.h"

namespace cx
{

ShadingWidget::ShadingWidget(QWidget* parent) :
  QWidget(parent),
  mLayout(new QVBoxLayout(this))
{
  this->setObjectName("ShadingWidget");
  this->setWindowTitle("Shading");

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
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)), this, SLOT(activeImageChangedSlot()));
  connect(mShadingCheckBox, SIGNAL(toggled(bool)), this, SLOT(shadingToggledSlot(bool)));

  QGridLayout* shadingLayput = new QGridLayout();
  shadingLayput->addWidget(mShadingCheckBox, 5,0); // if moved to correct place, it gets disabled... ?????????
  ssc::SliderGroupWidget* shadingAmbientWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingAmbient()), shadingLayput, 1);
  ssc::SliderGroupWidget* shadingDiffuseWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingDiffuse()), shadingLayput, 2);
  ssc::SliderGroupWidget* shadingSpecularWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecular()), shadingLayput, 3);
  ssc::SliderGroupWidget* shadingSpecularPowerWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecularPower()), shadingLayput, 4);

  shadingAmbientWidget->setEnabled(false);
  shadingDiffuseWidget->setEnabled(false);
  shadingSpecularWidget->setEnabled(false);
  shadingSpecularPowerWidget->setEnabled(false);

  mLayout->addLayout(shadingLayput);
  mLayout->addStretch(1);

//  this->setLayout(mLayout);
}

void ShadingWidget::shadingToggledSlot(bool val)
{
  std::cout << "shading attempt set to " << val << std::endl;
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (image)
  {
    image->setShadingOn(val);
    std::cout << "shading set to " << val << std::endl;
  }
}

void ShadingWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();

  if (activeImage)
  {
    std::cout << "shading updated to " << activeImage->getShadingOn() << std::endl;
    mShadingCheckBox->setChecked(activeImage->getShadingOn());
  }
}

}//namespace cx
