#include "cxToolPropertiesWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "UsConfigGui.h"
#include "cxDataInterface.h"
#include "cxToolManager.h"
#include "cxTool.h"
#include "cxToolDataAdapters.h"
#include "cxActiveToolWidget.h"
#include "sscManualTool.h"

namespace cx
{
  
ToolPropertiesWidget::ToolPropertiesWidget(QWidget* parent) :
  BaseWidget(parent, "ToolPropertiesWidget", "Tool Properties")
{
  //layout
  mToptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  mToptopLayout->addWidget(new ActiveToolWidget(this));
  
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
  connect(cxToolManager::getInstance()->getManualTool().get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(manualToolChanged()));
  connect(cxToolManager::getInstance()->getManualTool().get(), SIGNAL(toolVisible(bool)), this, SLOT(manualToolChanged()));
  connect(mManualToolWidget, SIGNAL(changed()), this, SLOT(manualToolWidgetChanged()));

  mSpaceSelector = StringDataAdapterXml::initialize("selectSpace",
      "Space",
      "Select coordinate system to store position in.",
      "",
	  QStringList(),
      QDomNode());
  connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spacesChangedSlot()));
  connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(setModified()));
  mSpaceSelector->setValue(SpaceHelpers::getPr().toString());
  manualGroupLayout->addWidget(new LabeledComboBoxWidget(this, mSpaceSelector));

  mUSSectorConfigBox = new LabeledComboBoxWidget(this, ActiveProbeConfigurationStringDataAdapter::New());
  mToptopLayout->addWidget(mUSSectorConfigBox);
  mUSSectorConfigBox->hide();

  QGridLayout* gridLayout = new QGridLayout;
  activeGroupLayout->addLayout(gridLayout);

  new SpinBoxAndSliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterActiveToolOffset), gridLayout, 0);

  mToptopLayout->addStretch();

  connect(toolManager(), SIGNAL(trackingStarted()), this, SLOT(referenceToolChangedSlot()));
  connect(toolManager(), SIGNAL(trackingStopped()), this, SLOT(referenceToolChangedSlot()));
  connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));

  connect(toolManager(), SIGNAL(configured()), this, SLOT(updateSlot()));
  connect(toolManager(), SIGNAL(initialized()), this, SLOT(updateSlot()));
  connect(toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateSlot()));
  connect(toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateSlot()));
  connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(updateSlot()));

  this->dominantToolChangedSlot();
  this->referenceToolChangedSlot();
  this->updateSlot();
  this->manualToolChanged();
  this->spacesChangedSlot();
}

ToolPropertiesWidget::~ToolPropertiesWidget()
{
}

QString ToolPropertiesWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tool Properties</h3>"
      "<p>TODO.</p>"
      "<p><i></i></p>"
      "</html>";
}

void ToolPropertiesWidget::manualToolChanged()
{
	if (!cxToolManager::getInstance()->getManualTool())
		return;
  mManualGroup->setVisible(cxToolManager::getInstance()->getManualTool()->getVisible());
  mManualToolWidget->blockSignals(true);

  Transform3D prMt = cxToolManager::getInstance()->getManualTool()->get_prMt();
  CoordinateSystem space_q = CoordinateSystem::fromString(mSpaceSelector->getValue());
  CoordinateSystem space_mt = SpaceHelpers::getTO(cxToolManager::getInstance()->getManualTool());
  Transform3D qMt = SpaceHelpers::get_toMfrom(space_mt, space_q);

  mManualToolWidget->setMatrix(qMt);
  mManualToolWidget->blockSignals(false);
}

void ToolPropertiesWidget::manualToolWidgetChanged()
{
	Transform3D qMt = mManualToolWidget->getMatrix();
  CoordinateSystem space_q = CoordinateSystem::fromString(mSpaceSelector->getValue());
  CoordinateSystem space_mt = SpaceHelpers::getTO(cxToolManager::getInstance()->getManualTool());
  CoordinateSystem space_pr = SpaceHelpers::getPr();
  Transform3D qMpr = SpaceHelpers::get_toMfrom(space_pr, space_q);
  Transform3D prMt = qMpr.inv() * qMt;

  cxToolManager::getInstance()->getManualTool()->set_prMt(prMt);
}

void ToolPropertiesWidget::spacesChangedSlot()
{
	CoordinateSystem space = CoordinateSystem::fromString(mSpaceSelector->getValue());

	std::vector<CoordinateSystem> spaces = SpaceHelpers::getAvailableSpaces(true);
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
	  range << spaces[i].toString();

	mSpaceSelector->setValueRange(range);
	mSpaceSelector->setValue(space.toString());
	mSpaceSelector->setHelp(QString("The space q to display tool position in,\n"
	                                "qMt"));
	this->setModified();
	this->manualToolChanged();
}

void ToolPropertiesWidget::dominantToolChangedSlot()
{
//  cxToolPtr cxTool = boost::dynamic_pointer_cast<cxTool>(mActiveTool);

  if (mActiveTool)
    disconnect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));

  mActiveTool = toolManager()->getDominantTool();

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

  mReferenceTool = toolManager()->getReferenceTool();

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
  if (toolManager()->isConfigured())
    status = "Configured";
  if (toolManager()->isInitialized())
    status = "Initialized";
  if (toolManager()->isTracking())
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
