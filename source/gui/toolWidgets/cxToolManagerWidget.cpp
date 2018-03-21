/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
