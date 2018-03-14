/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPrepareVesselsWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include "cxLabeledComboBoxWidget.h"
#include "cxTypeConversions.h"
#include "cxColorSelectButton.h"
#include "cxMesh.h"
#include "cxProfile.h"

#include "cxPipelineWidget.h"
#include "cxHelperWidgets.h"
#include "cxColorProperty.h"

#include "cxResampleImageFilter.h"
#include "cxSmoothingImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxRegistrationServiceProxy.h"

namespace cx
{
//------------------------------------------------------------------------------
PrepareVesselsWidget::PrepareVesselsWidget(RegServicesPtr services, QWidget* parent) :
		RegistrationBaseWidget(services, parent, "org_custusx_registration_method_vessel_prepare_widget", "PrepareVesselsWidget")
{  
	this->setToolTip("Prepare data for vessel I2I registration");

	XmlOptionFile options = profile()->getXmlSettings().descend("registration").descend("PrepareVesselsWidget");
  // fill the pipeline with filters:
	mPipeline.reset(new Pipeline(services->patient()));
  FilterGroupPtr filters(new FilterGroup(options.descend("pipeline")));
	filters->append(FilterPtr(new ResampleImageFilter(services)));
	filters->append(FilterPtr(new SmoothingImageFilter(services)));
	filters->append(FilterPtr(new BinaryThresholdImageFilter(services)));
	filters->append(FilterPtr(new BinaryThinningImageFilter3DFilter(services)));
  mPipeline->initialize(filters);

//  mPipeline->getNodes()[0]->setValueName("US Image:");
//  mPipeline->getNodes()[0]->setHelp("Select an US volume acquired from the wire phantom.");
  mPipeline->setOption("Color", QVariant(QColor("red")));

  mLayout = new QVBoxLayout(this);

  mPipelineWidget = new PipelineWidget(services->view(), services->patient(), NULL, mPipeline);
  mLayout->addWidget(mPipelineWidget);

  mColorProperty = ColorProperty::initialize("Color", "",
                                              "Color of all generated data.",
                                              QColor("green"), options.getElement());
  connect(mColorProperty.get(), SIGNAL(changed()), this, SLOT(setColorSlot()));

  QPushButton* fixedButton = new QPushButton("Set as Fixed");
  fixedButton->setToolTip("Set output of centerline generation as the Fixed Data in Registration");
  connect(fixedButton, SIGNAL(clicked()), this, SLOT(toFixedSlot()));
  QPushButton* movingButton = new QPushButton("Set as Moving");
  movingButton->setToolTip("Set output of centerline generation as the Moving Data in Registration");
  connect(movingButton, SIGNAL(clicked()), this, SLOT(toMovingSlot()));

  QLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(fixedButton);
  buttonsLayout->addWidget(movingButton);

	mLayout->addWidget(sscCreateDataWidget(this, mColorProperty));
  mLayout->addWidget(mPipelineWidget);
  mLayout->addStretch();
  mLayout->addLayout(buttonsLayout);
//  mLayout->addStretch();

  this->setColorSlot();

}

void PrepareVesselsWidget::setColorSlot()
{
  mPipeline->setOption(mColorProperty->getDisplayName(), QVariant(mColorProperty->getValue()));
}

void PrepareVesselsWidget::toMovingSlot()
{
	DataPtr data = mPipeline->getNodes().back()->getData();
	if (data)
		mServices->registration()->setMovingData(data);
}

void PrepareVesselsWidget::toFixedSlot()
{
	DataPtr data = mPipeline->getNodes().back()->getData();
	if (data)
		mServices->registration()->setFixedData(data);
}

PrepareVesselsWidget::~PrepareVesselsWidget()
{}


//------------------------------------------------------------------------------
}//namespace cx
