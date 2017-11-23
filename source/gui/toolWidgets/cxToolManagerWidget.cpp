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

#include <cxToolManagerWidget.h>

#include <QGridLayout>
#include <QPushButton>

#include "cxTrackingService.h"

namespace cx
{

ToolManagerWidget::ToolManagerWidget(TrackingServicePtr trackingService, QWidget* parent) :
	BaseWidget(parent, "tool_manager_widget", "ToolManager debugger"),
    mConfigureButton(new QPushButton("Configure")),
    mDeConfigureButton(new QPushButton("Deconfigure")),
    mInitializeButton(new QPushButton("Initialize")),
    mUnInitializeButton(new QPushButton("Uninitialize")),
    mStartTrackingButton(new QPushButton("Start Tracking")),
	mStopTrackingButton(new QPushButton("Stop Tracking")),
	mTrackingService(trackingService)
{
	this->setToolTip("ToolManager debugging utilities");
  //connect
  connect(mConfigureButton, SIGNAL(clicked(bool)), this, SLOT(configureClickedSlot(bool)));
  connect(mDeConfigureButton, SIGNAL(clicked(bool)), this, SLOT(deconfigureClickedSlot(bool)));
  connect(mInitializeButton, SIGNAL(clicked(bool)), this, SLOT(initializeClickedSlot(bool)));
  connect(mUnInitializeButton, SIGNAL(clicked(bool)), this, SLOT(uninitializeClickedSlot(bool)));
  connect(mStartTrackingButton, SIGNAL(clicked(bool)), this, SLOT(startTrackingClickedSlot(bool)));
  connect(mStopTrackingButton, SIGNAL(clicked(bool)), this, SLOT(stopTrackingClickedSlot(bool)));

  connect(mTrackingService.get(), &TrackingService::stateChanged, this, &ToolManagerWidget::updateButtonStatusSlot);

  //layout
  QGridLayout* layout = new QGridLayout(this);
  layout->addWidget(mConfigureButton, 0, 0);
  layout->addWidget(mDeConfigureButton, 0, 1);
  layout->addWidget(mInitializeButton, 1, 0);
  layout->addWidget(mUnInitializeButton, 1, 1);
  layout->addWidget(mStartTrackingButton, 2, 0);
  layout->addWidget(mStopTrackingButton, 2, 1);

  this->updateButtonStatusSlot();
}

ToolManagerWidget::~ToolManagerWidget()
{
}

void ToolManagerWidget::configureClickedSlot(bool checked)
{
	Q_UNUSED(checked);
  mTrackingService->setState(Tool::tsCONFIGURED);
}

void ToolManagerWidget::deconfigureClickedSlot(bool checked)
{
	Q_UNUSED(checked);
	mTrackingService->setState(Tool::tsNONE);
}

void ToolManagerWidget::initializeClickedSlot(bool checked)
{
	Q_UNUSED(checked);
	mTrackingService->setState(Tool::tsINITIALIZED);
}

void ToolManagerWidget::uninitializeClickedSlot(bool checked)
{
	Q_UNUSED(checked);
	mTrackingService->setState(Tool::tsCONFIGURED);
}

void ToolManagerWidget::startTrackingClickedSlot(bool checked)
{
	Q_UNUSED(checked);
	mTrackingService->setState(Tool::tsTRACKING);
}

void ToolManagerWidget::stopTrackingClickedSlot(bool checked)
{
	Q_UNUSED(checked);
	mTrackingService->setState(Tool::tsINITIALIZED);
}

void ToolManagerWidget::updateButtonStatusSlot()
{
	Tool::State state = mTrackingService->getState();
	mConfigureButton->setEnabled(state < Tool::tsCONFIGURED);
	mDeConfigureButton->setEnabled(state >= Tool::tsCONFIGURED);

	mInitializeButton->setEnabled(state < Tool::tsINITIALIZED);
	mUnInitializeButton->setEnabled(state >= Tool::tsINITIALIZED);

	mStartTrackingButton->setEnabled(state < Tool::tsTRACKING);
	mStopTrackingButton->setEnabled(state >= Tool::tsTRACKING);

}

}
