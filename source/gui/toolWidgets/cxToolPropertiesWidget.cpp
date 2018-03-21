/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxSpaceProvider.h"
#include "cxSpaceEditWidget.h"
#include "cxToolProperty.h"
#include "cxSpaceProvider.h"
#include "cxLogger.h"

namespace cx
{


ActiveToolPropertiesWidget::ActiveToolPropertiesWidget(TrackingServicePtr trackingService, SpaceProviderPtr spaceProvider, QWidget* parent) :
	BaseWidget(parent, "tool_properties_widget", "Tool Properties")
{

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	ActiveToolWidget* activeToolWidget = new ActiveToolWidget(trackingService, this);
	layout->addWidget(activeToolWidget);

	StringPropertyBasePtr selector = activeToolWidget->getSelector();

	ToolPropertiesWidget* info = new ToolPropertiesWidget(selector, trackingService, spaceProvider, this);
	info->layout()->setMargin(0);
	layout->addWidget(info);
}

ActiveToolPropertiesWidget::~ActiveToolPropertiesWidget()
{}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ToolPropertiesWidget::ToolPropertiesWidget(StringPropertyBasePtr toolSelector,
										   TrackingServicePtr trackingService,
										   SpaceProviderPtr spaceProvider,
										   QWidget* parent) :
  BaseWidget(parent, "ToolCorePropertiesWidget", "Tool Properties"),
  mSelector(toolSelector),
  mTrackingService(trackingService),
  mSpaceProvider(spaceProvider),
  mManualToolWidget(NULL)
{
	mToptopLayout = new QVBoxLayout(this);
	this->setModified();
}

ToolPropertiesWidget::~ToolPropertiesWidget()
{
}

void ToolPropertiesWidget::setupUI()
{
	if (mManualToolWidget) // already created
		return;

	this->setToolTip("Tool properties");
  //layout
  //toptopLayout->setMargin(0);

  QHBoxLayout* generalLayout = new QHBoxLayout;
  mReferenceStatusLabel = new QLabel("Reference frame <undefined>", this);
  generalLayout->addWidget(mReferenceStatusLabel);
  mTrackingSystemStatusLabel = new QLabel("Tracking <undefined>", this);
  generalLayout->addWidget(mTrackingSystemStatusLabel);

  mToptopLayout->addLayout(generalLayout);

  QGroupBox* activeGroup = new QGroupBox(this);
  activeGroup->setTitle("Tool");
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
  connect(mManualToolWidget, SIGNAL(changed()), this, SLOT(manualToolWidgetChanged()));

  mSpaceSelector = SpaceProperty::initialize("selectSpace",
	  "Space",
	  "Select coordinate system to store position in.");
  mSpaceSelector->setSpaceProvider(mSpaceProvider);
  connect(mSpaceSelector.get(), &SpaceProperty::valueWasSet, this, &ToolPropertiesWidget::spacesChangedSlot);
  connect(mSpaceSelector.get(), &SpaceProperty::valueWasSet, this, &ToolPropertiesWidget::setModified);
  mSpaceSelector->setValue(mSpaceProvider->getPr());
  manualGroupLayout->addWidget(new SpaceEditWidget(this, mSpaceSelector));

  TrackingServicePtr ts = mTrackingService;
  mUSSectorConfigBox = new LabeledComboBoxWidget(this, StringPropertyActiveProbeConfiguration::New(ts));
  mToptopLayout->addWidget(mUSSectorConfigBox);
  mUSSectorConfigBox->hide();

  QGridLayout* gridLayout = new QGridLayout;
  activeGroupLayout->addLayout(gridLayout);

  mToolOffset = DoublePropertyToolOffset::create(mTool);
  gridLayout->addWidget(new SpinBoxAndSliderGroupWidget(this, mToolOffset));

  mMetadataLabel = new QTextEdit;
  mToptopLayout->addWidget(mMetadataLabel);

  connect(ts.get(), &TrackingService::stateChanged, this, &ToolPropertiesWidget::reconnectTools);
  connect(mSelector.get(), &StringPropertyBase::changed, this, &ToolPropertiesWidget::activeToolChangedSlot);
  connect(ts.get(), &TrackingService::stateChanged, this, &ToolPropertiesWidget::setModified);

  this->reconnectTools();
  this->activeToolChangedSlot();
  this->spacesChangedSlot();
}

void ToolPropertiesWidget::prePaintEvent()
{
	this->setupUI();
	this->updateFrontend();
	this->toolPositionChanged();
}

void ToolPropertiesWidget::toolPositionChanged()
{
	if (!mTool)
		return;

	mManualGroup->setVisible(mTool->getVisible());
  mManualToolWidget->blockSignals(true);

  Transform3D prMt = mTool->get_prMt();
  CoordinateSystem space_q = mSpaceSelector->getValue();
	CoordinateSystem space_mt = mSpaceProvider->getT(mTool);
  Transform3D qMt = mSpaceProvider->get_toMfrom(space_mt, space_q);

  mManualToolWidget->setMatrix(qMt);
  mManualToolWidget->blockSignals(false);

  this->updateBrowser();
}

QString ToolPropertiesWidget::createDescriptionForTool(ToolPtr current)
{
	bool hasProbe = false;
	if(current->getProbe())
		hasProbe=true;
	QString meta = current->getMetadata().toString();
	return QString("Tool=%1: visible=%2 has_probe=%3\n%4\n")
			.arg(current->getName())
			.arg(current->getVisible())
			.arg(hasProbe)
			.arg(meta);
}

void ToolPropertiesWidget::updateBrowser()
{
	QString text;

	for (TrackingService::ToolMap::iterator i=mTools.begin(); i!=mTools.end(); ++i)
	{
		text += this->createDescriptionForTool(i->second) + "\n";
	}

	int sstart = mMetadataLabel->textCursor().selectionStart();
	int send = mMetadataLabel->textCursor().selectionEnd();
	int textPos = mMetadataLabel->textCursor().position();

	mMetadataLabel->setPlainText(text);
	QTextCursor cursor = mMetadataLabel->textCursor();

	cursor.setPosition(sstart, QTextCursor::MoveAnchor);
	cursor.setPosition(send, QTextCursor::KeepAnchor);

	mMetadataLabel->setTextCursor(cursor);
}

void ToolPropertiesWidget::manualToolWidgetChanged()
{
	if (!mTool)
		return;

	Transform3D qMt = mManualToolWidget->getMatrix();
  CoordinateSystem space_q = mSpaceSelector->getValue();
//  CoordinateSystem space_mt = mSpaceProvider->getTO(mTool);
  CoordinateSystem space_pr = mSpaceProvider->getPr();
  Transform3D qMpr = mSpaceProvider->get_toMfrom(space_pr, space_q);
  Transform3D prMt = qMpr.inv() * qMt;

  mTool->set_prMt(prMt);
}

void ToolPropertiesWidget::spacesChangedSlot()
{
	CoordinateSystem space = mSpaceSelector->getValue();

//	mSpaceSelector->setValueRange(spaceProvider()->getSpacesToPresentInGUI());
	mSpaceSelector->setValue(space);
	mSpaceSelector->setHelp(QString("The space q to display tool position in,\n"
	                                "qMt"));
	this->setModified();
//	this->toolPositionChanged();
}

void ToolPropertiesWidget::reconnectTools()
{
	for (TrackingService::ToolMap::iterator i=mTools.begin(); i!=mTools.end(); ++i)
	{
		disconnect(i->second.get(), &Tool::toolVisible, this, &ToolPropertiesWidget::setModified);
		disconnect(i->second.get(), &Tool::toolTransformAndTimestamp, this, &ToolPropertiesWidget::setModified);
	}
	mTools = mTrackingService->getTools();
	for (TrackingService::ToolMap::iterator i=mTools.begin(); i!=mTools.end(); ++i)
	{
		connect(i->second.get(), &Tool::toolVisible, this, &ToolPropertiesWidget::setModified);
		connect(i->second.get(), &Tool::toolTransformAndTimestamp, this, &ToolPropertiesWidget::setModified);
	}
}

void ToolPropertiesWidget::activeToolChangedSlot()
{
  mTool = mTrackingService->getTool(mSelector->getValue());

  mToolOffset->setTool(mTool);
  mUSSectorConfigBox->setVisible(mTool && mTool->hasType(Tool::TOOL_US_PROBE));
  mToptopLayout->update();

  this->setModified();
}

void ToolPropertiesWidget::updateFrontend()
{
  if (mTool)
  {
	mToolNameLabel->setText(qstring_cast(mTool->getName()));
	QString text = mTool->getVisible() ? "Visible" : "Not Visible";
    mActiveToolVisibleLabel->setText(text);
  }
  else
  {
    mToolNameLabel->setText("none");
    mActiveToolVisibleLabel->setText("");
  }

  ToolPtr reference = mTrackingService->getReferenceTool();
  if (reference)
  {
	QString text = reference->getVisible() ? "Visible" : "Not Visible";
    mReferenceStatusLabel->setText("Reference " + text);
  }
  else
  {
    mReferenceStatusLabel->setText("Reference is the tracker");
  }

  QString status = "Unconfigured";
  if (mTrackingService->getState()==Tool::tsCONFIGURED)
    status = "Configured";
  if (mTrackingService->getState()==Tool::tsINITIALIZED)
	status = "Initialized";
  if (mTrackingService->getState()==Tool::tsTRACKING)
	status = "Tracking";
  mTrackingSystemStatusLabel->setText("Tracking status: " + status);
}

}//end namespace cx
