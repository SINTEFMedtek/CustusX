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

#include <cxToolFilterWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include "cxEnumConverter.h"
#include "cxStateService.h"
#include "cxToolManager.h"
#include "cxSelectionGroupBox.h"
#include "cxToolListWidget.h"
#include "cxApplicationStateMachine.h"

namespace cx
{

ToolFilterGroupBox::ToolFilterGroupBox(QWidget* parent) :
    QGroupBox(parent)
{
  this->setTitle("Tool filter");

  mApplicationGroupBox = new SelectionGroupBox("Applications", stateService()->getApplication()->getAllApplicationNames(), Qt::Vertical, false, NULL);
  mApplicationGroupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);

  mTrackingSystemGroupBox = new SelectionGroupBox("Tracking systems", trackingService()->getSupportedTrackingSystems(), Qt::Horizontal, true, NULL);
  mToolListWidget = new FilteringToolListWidget(NULL);
  connect(mToolListWidget, SIGNAL(toolSelected(QString)), this, SIGNAL(toolSelected(QString)));

  QGridLayout* layout = new QGridLayout(this);
  layout->addWidget(mApplicationGroupBox);
  connect(mApplicationGroupBox, SIGNAL(selectionChanged()), this, SLOT(filterSlot()));
  layout->addWidget(mTrackingSystemGroupBox);
  connect(mTrackingSystemGroupBox, SIGNAL(selectionChanged()), this, SLOT(filterSlot()));

  QGroupBox* toolGroupBox = new QGroupBox();
  toolGroupBox->setTitle("Tools");
  QVBoxLayout* toolLayout = new QVBoxLayout();
  toolGroupBox->setLayout(toolLayout);
  toolLayout->addWidget(mToolListWidget);

  layout->addWidget(toolGroupBox);
}

ToolFilterGroupBox::~ToolFilterGroupBox()
{}

void ToolFilterGroupBox::setClinicalApplicationSlot(CLINICAL_APPLICATION clinicalApplication)
{
  QStringList selectedApplication;
  selectedApplication << enum2string(clinicalApplication);
  mApplicationGroupBox->setSelected(selectedApplication);
}

void ToolFilterGroupBox::setTrackingSystemSlot(TRACKING_SYSTEM trackingSystem)
{
  QStringList selectedTrackingSystem;
  selectedTrackingSystem << enum2string(trackingSystem);
  mTrackingSystemGroupBox->setSelected(selectedTrackingSystem);
}

void ToolFilterGroupBox::filterSlot()
{
  QStringList applicationFilter = mApplicationGroupBox->getSelected();
  QStringList trackingSystemFilter = mTrackingSystemGroupBox->getSelected();

  mToolListWidget->filterSlot(applicationFilter, trackingSystemFilter);
}
}//namespace cx
