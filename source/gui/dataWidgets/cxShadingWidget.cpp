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
  ssc::SliderGroupWidget* shadingAmbientWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingAmbient()), shadingLayput, 1);
  ssc::SliderGroupWidget* shadingDiffuseWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingDiffuse()), shadingLayput, 2);
  ssc::SliderGroupWidget* shadingSpecularWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecular()), shadingLayput, 3);
  ssc::SliderGroupWidget* shadingSpecularPowerWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecularPower()), shadingLayput, 4);

  shadingAmbientWidget->setEnabled(false);
  shadingDiffuseWidget->setEnabled(false);
  shadingSpecularWidget->setEnabled(false);
  shadingSpecularPowerWidget->setEnabled(false);

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mLayout->addLayout(shadingLayput);
  mLayout->addStretch(1);

//  this->setLayout(mLayout);
}

void ShadingWidget::shadingToggledSlot(bool val)
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (image)
  {
    image->setShadingOn(val);
  }
}

void ShadingWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();

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
