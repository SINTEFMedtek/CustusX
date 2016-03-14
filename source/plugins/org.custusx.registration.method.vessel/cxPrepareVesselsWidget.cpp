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
