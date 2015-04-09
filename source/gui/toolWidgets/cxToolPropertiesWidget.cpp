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

#include "cxToolPropertiesWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>


#include "cxTrackingService.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTypeConversions.h"
//#include "UsConfigGui.h"
#include "cxDataInterface.h"
#include "cxTrackingService.h"
#include "cxTool.h"
#include "cxToolProperty.h"
#include "cxActiveToolWidget.h"
#include "cxManualTool.h"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"
#include "cxSpaceEditWidget.h"
#include "cxLegacySingletons.h"

namespace cx
{
  
ToolPropertiesWidget::ToolPropertiesWidget(QWidget* parent) :
  BaseWidget(parent, "ToolPropertiesWidget", "Tool Properties")
{
	this->setToolTip("Tool properties");
  //layout
  mToptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  mToptopLayout->addWidget(new ActiveToolWidget(trackingService(), this));
  
  QHBoxLayout* generalLayout = new QHBoxLayout;
  mReferenceStatusLabel = new QLabel("Reference frame <undefined>", this);
  generalLayout->addWidget(mReferenceStatusLabel);
  mTrackingSystemStatusLabel = new QLabel("Tracking <undefined>", this);
  generalLayout->addWidget(mTrackingSystemStatusLabel);

  mToptopLayout->addLayout(generalLayout);

  QGroupBox* activeGroup = new QGroupBox(this);
  activeGroup->setTitle("Active Tool");
  mToptopLayout->addWidget(activeGroup);
  QVBoxLayout* activeGroupLayout = new QVBoxLayout;
  activeGroup->setLayout(activeGroupLayout);

  QHBoxLayout* activeToolLayout = new QHBoxLayout;
  activeToolLayout->addWidget(new QLabel("Name:", this));
  mToolNameLabel = new QLabel(this);
  activeToolLayout->addWidget(mToolNameLabel);
  mActiveToolVisibleLabel = new QLabel("Visible: NA");
  activeToolLayout->addWidget(mActiveToolVisibleLabel);
  activeGroupLayout->addLayout(activeToolLayout);

  QGroupBox* manualGroup = new QGroupBox(this);
  manualGroup->setTitle("Manual Tool");
  mToptopLayout->addWidget(manualGroup);
  QVBoxLayout* manualGroupLayout = new QVBoxLayout;
  mManualGroup = manualGroup;
  manualGroup->setLayout(manualGroupLayout);
  manualGroupLayout->setMargin(0);
  mManualToolWidget = new Transform3DWidget(manualGroup);
  manualGroupLayout->addWidget(mManualToolWidget);
  connect(trackingService()->getManualTool().get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(manualToolChanged()));
  connect(trackingService()->getManualTool().get(), SIGNAL(toolVisible(bool)), this, SLOT(manualToolChanged()));
  connect(mManualToolWidget, SIGNAL(changed()), this, SLOT(manualToolWidgetChanged()));

  mSpaceSelector = SpaceProperty::initialize("selectSpace",
      "Space",
	  "Select coordinate system to store position in.");
  mSpaceSelector->setSpaceProvider(spaceProvider());
  connect(mSpaceSelector.get(), &SpaceProperty::valueWasSet, this, &ToolPropertiesWidget::spacesChangedSlot);
  connect(mSpaceSelector.get(), &SpaceProperty::valueWasSet, this, &ToolPropertiesWidget::setModified);
  mSpaceSelector->setValue(spaceProvider()->getPr());
  manualGroupLayout->addWidget(new SpaceEditWidget(this, mSpaceSelector));

  TrackingServicePtr ts = trackingService();
  mUSSectorConfigBox = new LabeledComboBoxWidget(this, StringPropertyActiveProbeConfiguration::New(ts));
  mToptopLayout->addWidget(mUSSectorConfigBox);
  mUSSectorConfigBox->hide();

  QGridLayout* gridLayout = new QGridLayout;
  activeGroupLayout->addLayout(gridLayout);

  new SpinBoxAndSliderGroupWidget(this, DoublePropertyBasePtr(new DoublePropertyActiveToolOffset), gridLayout, 0);

  mToptopLayout->addStretch();

  connect(trackingService().get(), &TrackingService::stateChanged, this, &ToolPropertiesWidget::referenceToolChangedSlot);

  connect(trackingService().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(activeToolChangedSlot()));

  connect(trackingService().get(), &TrackingService::stateChanged, this, &ToolPropertiesWidget::updateSlot);
  connect(trackingService().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(updateSlot()));

  this->activeToolChangedSlot();
  this->referenceToolChangedSlot();
  this->updateSlot();
  this->manualToolChanged();
  this->spacesChangedSlot();
}

ToolPropertiesWidget::~ToolPropertiesWidget()
{
}

void ToolPropertiesWidget::manualToolChanged()
{
	if (!trackingService()->getManualTool())
		return;
  mManualGroup->setVisible(trackingService()->getManualTool()->getVisible());
  mManualToolWidget->blockSignals(true);

  Transform3D prMt = trackingService()->getManualTool()->get_prMt();
  CoordinateSystem space_q = mSpaceSelector->getValue();
  CoordinateSystem space_mt = spaceProvider()->getTO(trackingService()->getManualTool());
  Transform3D qMt = spaceProvider()->get_toMfrom(space_mt, space_q);

  mManualToolWidget->setMatrix(qMt);
  mManualToolWidget->blockSignals(false);
}

void ToolPropertiesWidget::manualToolWidgetChanged()
{
	Transform3D qMt = mManualToolWidget->getMatrix();
  CoordinateSystem space_q = mSpaceSelector->getValue();
  CoordinateSystem space_mt = spaceProvider()->getTO(trackingService()->getManualTool());
  CoordinateSystem space_pr = spaceProvider()->getPr();
  Transform3D qMpr = spaceProvider()->get_toMfrom(space_pr, space_q);
  Transform3D prMt = qMpr.inv() * qMt;

  trackingService()->getManualTool()->set_prMt(prMt);
}

void ToolPropertiesWidget::spacesChangedSlot()
{
	CoordinateSystem space = mSpaceSelector->getValue();

//	mSpaceSelector->setValueRange(spaceProvider()->getSpacesToPresentInGUI());
	mSpaceSelector->setValue(space);
	mSpaceSelector->setHelp(QString("The space q to display tool position in,\n"
	                                "qMt"));
	this->setModified();
	this->manualToolChanged();
}

void ToolPropertiesWidget::activeToolChangedSlot()
{
  if (mActiveTool)
    disconnect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));

  mActiveTool = trackingService()->getActiveTool();

  if(mActiveTool && mActiveTool->hasType(Tool::TOOL_US_PROBE))
  {
    mUSSectorConfigBox->show();
    mToptopLayout->update();
  }
  else
  {
    mUSSectorConfigBox->hide();
    mToptopLayout->update();
  }

  if (mActiveTool)
    connect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));
}

void ToolPropertiesWidget::referenceToolChangedSlot()
{
  if (mReferenceTool)
    disconnect(mReferenceTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));

  mReferenceTool = trackingService()->getReferenceTool();

  if (mReferenceTool)
    connect(mReferenceTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));
}

void ToolPropertiesWidget::updateSlot()
{
  if (mActiveTool)
  {
    mToolNameLabel->setText(qstring_cast(mActiveTool->getName()));
    QString text = mActiveTool->getVisible() ? "Visible" : "Not Visible";
    mActiveToolVisibleLabel->setText(text);
  }
  else
  {
    mToolNameLabel->setText("none");
    mActiveToolVisibleLabel->setText("");
  }

  if (mReferenceTool)
  {
    QString text = mReferenceTool->getVisible() ? "visible" : "not visible";
    mReferenceStatusLabel->setText("Reference " + text);
  }
  else
  {
    mReferenceStatusLabel->setText("Reference is the tracker");
  }

  QString status = "Unconfigured";
  if (trackingService()->getState()==Tool::tsCONFIGURED)
    status = "Configured";
  if (trackingService()->getState()==Tool::tsINITIALIZED)
	status = "Initialized";
  if (trackingService()->getState()==Tool::tsTRACKING)
	status = "Tracking";
  mTrackingSystemStatusLabel->setText("Tracking status: " + status);
}

void ToolPropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ToolPropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx
