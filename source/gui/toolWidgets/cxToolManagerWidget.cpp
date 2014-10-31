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

#include "cxToolManager.h"

namespace cx
{

ToolManagerWidget::ToolManagerWidget(QWidget* parent) :
    BaseWidget(parent, "ToolManagerWidget", "ToolManager debugger"),
    mConfigureButton(new QPushButton("Configure")),
    mDeConfigureButton(new QPushButton("Deconfigure")),
    mInitializeButton(new QPushButton("Initialize")),
    mUnInitializeButton(new QPushButton("Uninitialize")),
    mStartTrackingButton(new QPushButton("Start Tracking")),
    mStopTrackingButton(new QPushButton("Stop Tracking"))
{
  //connect
  connect(mConfigureButton, SIGNAL(clicked(bool)), this, SLOT(configureClickedSlot(bool)));
  connect(mDeConfigureButton, SIGNAL(clicked(bool)), this, SLOT(deconfigureClickedSlot(bool)));
  connect(mInitializeButton, SIGNAL(clicked(bool)), this, SLOT(initializeClickedSlot(bool)));
  connect(mUnInitializeButton, SIGNAL(clicked(bool)), this, SLOT(uninitializeClickedSlot(bool)));
  connect(mStartTrackingButton, SIGNAL(clicked(bool)), this, SLOT(startTrackingClickedSlot(bool)));
  connect(mStopTrackingButton, SIGNAL(clicked(bool)), this, SLOT(stopTrackingClickedSlot(bool)));

//  connect(toolManager(), SIGNAL(configured()), this, SLOT(updateButtonStatusSlot()));
//  connect(toolManager(), SIGNAL(deconfigured()), this, SLOT(updateButtonStatusSlot()));
//  connect(toolManager(), SIGNAL(initialized()), this, SLOT(updateButtonStatusSlot()));
//  connect(toolManager(), SIGNAL(uninitialized()), this, SLOT(updateButtonStatusSlot()));
//  connect(toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateButtonStatusSlot()));
//  connect(toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateButtonStatusSlot()));
  connect(toolManager(), &ToolManager::stateChanged, this, &ToolManagerWidget::updateButtonStatusSlot);

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

QString ToolManagerWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>ToolManager debugging utilities.</h3>"
      "<p>Lets you test different aspects of the toolmanager.</p>"
      "<p><i></i></p>"
      "</html>";
}

void ToolManagerWidget::configureClickedSlot(bool checked)
{
  toolManager()->setState(Tool::tsCONFIGURED);
}

void ToolManagerWidget::deconfigureClickedSlot(bool checked)
{
	toolManager()->setState(Tool::tsNONE);
//  toolManager()->deconfigure();
}

void ToolManagerWidget::initializeClickedSlot(bool checked)
{
	toolManager()->setState(Tool::tsINITIALIZED);
//  toolManager()->initialize();
}

void ToolManagerWidget::uninitializeClickedSlot(bool checked)
{
	toolManager()->setState(Tool::tsCONFIGURED);
//  toolManager()->uninitialize();
}

void ToolManagerWidget::startTrackingClickedSlot(bool checked)
{
	toolManager()->setState(Tool::tsTRACKING);
//  toolManager()->startTracking();
}

void ToolManagerWidget::stopTrackingClickedSlot(bool checked)
{
	toolManager()->setState(Tool::tsINITIALIZED);
//  toolManager()->stopTracking();
}

void ToolManagerWidget::updateButtonStatusSlot()
{
	mConfigureButton->setEnabled(toolManager()->getState() < Tool::tsCONFIGURED);
	mDeConfigureButton->setEnabled(toolManager()->getState() >= Tool::tsCONFIGURED);

	mInitializeButton->setEnabled(toolManager()->getState() < Tool::tsINITIALIZED);
	mUnInitializeButton->setEnabled(toolManager()->getState() >= Tool::tsINITIALIZED);

	mStartTrackingButton->setEnabled(toolManager()->getState() < Tool::tsTRACKING);
	mStopTrackingButton->setEnabled(toolManager()->getState() >= Tool::tsTRACKING);

//  mConfigureButton->setDisabled(toolManager()->isConfigured());
//  mDeConfigureButton->setDisabled(!toolManager()->isConfigured());
//  mInitializeButton->setDisabled(toolManager()->isInitialized());
//  mUnInitializeButton->setDisabled(!toolManager()->isInitialized());
//  mStartTrackingButton->setDisabled(toolManager()->isTracking());
//  mStopTrackingButton->setDisabled(!toolManager()->isTracking());
}
}
