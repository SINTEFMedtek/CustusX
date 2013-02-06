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
  manualGroup->setLayout(manualGroupLayout);
  manualGroupLayout->setMargin(0);
  mManualToolWidget = new Transform3DWidget(manualGroup);
  manualGroupLayout->addWidget(mManualToolWidget);
  connect(ToolManager::getInstance()->getManualTool().get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(manualToolChanged()));
  connect(ToolManager::getInstance()->getManualTool().get(), SIGNAL(toolVisible(bool)), this, SLOT(manualToolChanged()));
  connect(mManualToolWidget, SIGNAL(changed()), this, SLOT(manualToolWidgetChanged()));

  mSpaceSelector = ssc::StringDataAdapterXml::initialize("selectSpace",
      "Space",
      "Select coordinate system to store position in.",
      "",
	  QStringList(),
      QDomNode());
  connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spacesChangedSlot()));
  connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(setModified()));
  mSpaceSelector->setValue(ssc::SpaceHelpers::getPr().toString());
  manualGroupLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mSpaceSelector));
  this->spacesChangedSlot();

  mManualGroup = manualGroup;
  this->manualToolChanged();
  
  mUSSectorConfigBox = new ssc::LabeledComboBoxWidget(this, ActiveProbeConfigurationStringDataAdapter::New());
  mToptopLayout->addWidget(mUSSectorConfigBox);
  mUSSectorConfigBox->hide();

  QGridLayout* gridLayout = new QGridLayout;
  activeGroupLayout->addLayout(gridLayout);

  new ssc::SpinBoxAndSliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterActiveToolOffset), gridLayout, 0);

  mToptopLayout->addStretch();

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(referenceToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(referenceToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));

  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(updateSlot()));

  dominantToolChangedSlot();
  referenceToolChangedSlot();
  updateSlot();
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
  mManualGroup->setVisible(ToolManager::getInstance()->getManualTool()->getVisible());
  mManualToolWidget->blockSignals(true);

  ssc::Transform3D prMt = ToolManager::getInstance()->getManualTool()->get_prMt();
  ssc::CoordinateSystem space_q = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
  ssc::CoordinateSystem space_mt = ssc::SpaceHelpers::getTO(ToolManager::getInstance()->getManualTool());
  ssc::Transform3D qMt = ssc::SpaceHelpers::get_toMfrom(space_mt, space_q);

  mManualToolWidget->setMatrix(qMt);
  mManualToolWidget->blockSignals(false);
}

void ToolPropertiesWidget::manualToolWidgetChanged()
{
	ssc::Transform3D qMt = mManualToolWidget->getMatrix();
  ssc::CoordinateSystem space_q = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
  ssc::CoordinateSystem space_mt = ssc::SpaceHelpers::getTO(ToolManager::getInstance()->getManualTool());
  ssc::CoordinateSystem space_pr = ssc::SpaceHelpers::getPr();
  ssc::Transform3D qMpr = ssc::SpaceHelpers::get_toMfrom(space_pr, space_q);
  ssc::Transform3D prMt = qMpr.inv() * qMt;

  ToolManager::getInstance()->getManualTool()->set_prMt(prMt);
}

void ToolPropertiesWidget::spacesChangedSlot()
{
	ssc::CoordinateSystem space = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());

	std::vector<ssc::CoordinateSystem> spaces = ssc::SpaceHelpers::getAvailableSpaces(true);
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
	  range << spaces[i].toString();

	mSpaceSelector->setValueRange(range);
	mSpaceSelector->setValue(space.toString());
	mSpaceSelector->setHelp(QString("The space q to display tool position in,\n"
	                                "qMt"));
//	mListener->setSpace(space);
	this->setModified();
}

void ToolPropertiesWidget::dominantToolChangedSlot()
{
  ToolPtr cxTool = boost::shared_dynamic_cast<Tool>(mActiveTool);

  if (mActiveTool)
    disconnect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));

  mActiveTool = ssc::toolManager()->getDominantTool();

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

  mReferenceTool = ssc::toolManager()->getReferenceTool();

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
  if (ssc::toolManager()->isConfigured())
    status = "Configured";
  if (ssc::toolManager()->isInitialized())
    status = "Initialized";
  if (ssc::toolManager()->isTracking())
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
