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

#include "cxConnectedThresholdImageFilterWidget.h"

#include <QPushButton>

#include "cxDataManager.h"
#include "cxPatientData.h"
#include "cxPatientService.h"

namespace cx
{

ConnectedThresholdImageFilterWidget::ConnectedThresholdImageFilterWidget(QWidget* parent) :
    BaseWidget(parent, "ConnectedThresholdImageFilterWidget", "Connected Threshold Image Filter"),
    mStatusLabel(new QLabel(""))
{
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  connect(&mConnectedThresholdImageFilter, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));

  QPushButton* segmentButton = new QPushButton("Segment", this);
  connect(segmentButton, SIGNAL(clicked()), this, SLOT(segmentSlot()));

  QPushButton* segmentationOptionsButton = new QPushButton("Options", this);
  segmentationOptionsButton->setCheckable(true);

  QGroupBox* segmentationOptionsWidget = this->createGroupbox(this->createSegmentationOptionsWidget(), "Segmentation options");
  connect(segmentationOptionsButton, SIGNAL(clicked(bool)), segmentationOptionsWidget, SLOT(setVisible(bool)));
  segmentationOptionsWidget->setVisible(segmentationOptionsButton->isChecked());

  topLayout->addWidget(segmentButton, 1,0);
  topLayout->addWidget(segmentationOptionsButton, 1,1);
  topLayout->addWidget(segmentationOptionsWidget, 2, 0, 1, 2);
  topLayout->addWidget(mStatusLabel);
}

ConnectedThresholdImageFilterWidget::~ConnectedThresholdImageFilterWidget()
{
}

QString ConnectedThresholdImageFilterWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Connected Threshold Image Filter.</h3>"
      "<p>Labels pixels with a ReplaceValue that are connected to an initial seed and lies within a lower and upper threshold range.</p>"
      "<p><i></i></p>"
      "</html>";
}

void ConnectedThresholdImageFilterWidget::handleFinishedSlot()
{
  ImagePtr segmentedImage = mConnectedThresholdImageFilter.getOutput();
  if(!segmentedImage)
    return;

  mStatusLabel->setText("<font color=green> Done. </font>\n");
}

void ConnectedThresholdImageFilterWidget::segmentSlot()
{
  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();

  //TODO add user interface
  ImagePtr selectedImage = dataManager()->getActiveImage();
  float lowerThreshold = 50.0;
  float upperThreshold = 647.0;
  int replaceValue = 150;
  itkImageType::IndexType seed;
  seed[0] = 88;
  seed[1] = 134;
  seed[2] = 14;
  //hack

  mConnectedThresholdImageFilter.setInput(selectedImage, outputBasePath, lowerThreshold, upperThreshold, replaceValue, seed);

  mStatusLabel->setText("<font color=orange> Generating segmentation... Please wait!</font>\n");
}

QWidget* ConnectedThresholdImageFilterWidget::createSegmentationOptionsWidget()
{
  QWidget* retval = new QWidget();

  //TODO add gui for parameters to the algorithm

  return retval;
}
}
