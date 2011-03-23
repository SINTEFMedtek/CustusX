/*
 * cxToolWidget.cpp
 *
 *  Created on: Apr 22, 2010
 *      Author: christiana
 */

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


namespace cx
{

ActiveToolStringDataAdapter::ActiveToolStringDataAdapter()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
  connect(ssc::toolManager(), SIGNAL(configured()), this, SIGNAL(changed()));
}

QString ActiveToolStringDataAdapter::getValueName() const
{
  return "Active Tool";
}
bool ActiveToolStringDataAdapter::setValue(const QString& value)
{
  ssc::ToolPtr newTool = ssc::toolManager()->getTool(value);
  if(newTool == ssc::toolManager()->getDominantTool())
    return false;
  ssc::toolManager()->setDominantTool(newTool->getUid());
  return true;
}
QString ActiveToolStringDataAdapter::getValue() const
{
  if (!ssc::toolManager()->getDominantTool())
    return "";
  return qstring_cast(ssc::toolManager()->getDominantTool()->getUid());
}
QString ActiveToolStringDataAdapter::getHelp() const
{
  return "select the active (dominant) tool";
}
QStringList ActiveToolStringDataAdapter::getValueRange() const
{
  std::vector<QString> uids = ssc::toolManager()->getToolUids();
  QStringList retval;
  //retval << ""; //Don't add "no tool" choice
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}
QString ActiveToolStringDataAdapter::convertInternal2Display(QString internal)
{
  ssc::ToolPtr tool = ssc::toolManager()->getTool(internal);
  if (!tool)
    return "<no tool>";
  return qstring_cast(tool->getName());
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

ActiveToolWidget::ActiveToolWidget(QWidget* parent) :
QWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("ActiveToolWidget");
  layout->setMargin(0);
  
  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, ActiveToolStringDataAdapter::New());
  layout->addWidget(combo);
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


ActiveToolConfigurationStringDataAdapter::ActiveToolConfigurationStringDataAdapter()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
}
void ActiveToolConfigurationStringDataAdapter::dominantToolChanged()
{
  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ssc::ToolPtr newTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());
  if (!newTool || newTool->getProbeSector().mType==ssc::ProbeData::tNONE)
    return;

  if (mTool)
    disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  mTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());

  if (mTool)
    connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  emit changed();
}
QString ActiveToolConfigurationStringDataAdapter::getValueName() const
{
  return "Probe Config";
}
bool ActiveToolConfigurationStringDataAdapter::setValue(const QString& value)
{
  if (!mTool)
    return false;
  mTool->getProbe()->setConfigId(value);
  return true;
}
QString ActiveToolConfigurationStringDataAdapter::getValue() const
{
  if (!mTool)
    return "";
  return mTool->getProbe()->getConfigId();
}
QString ActiveToolConfigurationStringDataAdapter::getHelp() const
{
  return "Select a probe configuration for the active tool.";
}
QStringList ActiveToolConfigurationStringDataAdapter::getValueRange() const
{
  if (!mTool)
    return QStringList();
  return mTool->getProbe()->getConfigIdList();
}
QString ActiveToolConfigurationStringDataAdapter::convertInternal2Display(QString internal)
{
  if (!mTool)
    return "<no tool>";
  return mTool->getProbe()->getConfigName(internal); ///< get a name for the given configuration
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

  
ToolPropertiesWidget::ToolPropertiesWidget(QWidget* parent) :
  QWidget(parent)
{
  this->setObjectName("ToolPropertiesWidget");
  this->setWindowTitle("Tool Properties");

//  ssc::Frame3D().test();

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
  mManualToolWidget = new AffineMatrixWidget(manualGroup);
  manualGroupLayout->addWidget(mManualToolWidget);
  connect(ToolManager::getInstance()->getManualTool().get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(manualToolChanged()));
  connect(ToolManager::getInstance()->getManualTool().get(), SIGNAL(toolVisible(bool)), this, SLOT(manualToolChanged()));
  connect(mManualToolWidget, SIGNAL(changed()), this, SLOT(manualToolWidgetChanged()));
  mManualGroup = manualGroup;
  this->manualToolChanged();
  
  //TODO: Add enable/disable US Probe visualization in 2D/3D?
  //TODO: Only show US probe properties if tool is US Probe

  mUSSectorConfigBox = new ssc::LabeledComboBoxWidget(this, ActiveToolConfigurationStringDataAdapter::New());
  mToptopLayout->addWidget(mUSSectorConfigBox);
  mUSSectorConfigBox->hide();

//  mUSSectorConfigBox = new QComboBox(this);
//  mUSSectorConfigLabel = new QLabel("Probe sector configuration:", this);
//  mToptopLayout->addWidget(mUSSectorConfigLabel);
//  mToptopLayout->addWidget(mUSSectorConfigBox);
//  mUSSectorConfigLabel->hide();
//  mUSSectorConfigBox->hide();

//  QGroupBox* group2D = new QGroupBox(this);
//  group2D->setTitle("2D properties");
//  mToptopLayout->addWidget(group2D);

  QGridLayout* gridLayout = new QGridLayout;
  activeGroupLayout->addLayout(gridLayout);

  mToolOffsetWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterActiveToolOffset), gridLayout, 0);

  mToptopLayout->addStretch();

//  void configured(); ///< signal emitted when the system is configured
//  void initialized(); ///< signal emitted when the system is initialized
//  void trackingStarted(); ///< signal emitted when the system starts tracking
//  void trackingStopped(); ///< signal emitted when the system stops tracking

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(referenceToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(referenceToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));

  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(updateSlot()));

//  connect(mUSSectorConfigBox, SIGNAL(currentIndexChanged(int)), this, SLOT(configurationChangedSlot(int)));

  dominantToolChangedSlot();
  referenceToolChangedSlot();
  updateSlot();
}

ToolPropertiesWidget::~ToolPropertiesWidget()
{
}

void ToolPropertiesWidget::manualToolChanged()
{
  mManualGroup->setVisible(ToolManager::getInstance()->getManualTool()->getVisible());
  mManualToolWidget->setMatrix(ToolManager::getInstance()->getManualTool()->get_prMt());
}

void ToolPropertiesWidget::manualToolWidgetChanged()
{
  ssc::Transform3D M = mManualToolWidget->getMatrix();
  ToolManager::getInstance()->getManualTool()->set_prMt(M);
}

void ToolPropertiesWidget::dominantToolChangedSlot()
{
  ToolPtr cxTool = boost::shared_dynamic_cast<Tool>(mActiveTool);

  if (mActiveTool)
    disconnect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));

  mActiveTool = ssc::toolManager()->getDominantTool();

  if(mActiveTool && mActiveTool->getType() == ssc::Tool::TOOL_US_PROBE)
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
