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
  if (mTool)
    disconnect(mTool.get(), SIGNAL(probeSectorConfigurationChanged()), this, SIGNAL(changed()));

  mTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());

  if (mTool)
    connect(mTool.get(), SIGNAL(probeSectorConfigurationChanged()), this, SIGNAL(changed()));

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
  mTool->setProbeSectorConfigIdString(value);
  return true;
}
QString ActiveToolConfigurationStringDataAdapter::getValue() const
{
  if (!mTool)
    return "";
  return mTool->getProbeSectorConfigIdString();
}
QString ActiveToolConfigurationStringDataAdapter::getHelp() const
{
  return "Select a probe configuration for the active tool.";
}
QStringList ActiveToolConfigurationStringDataAdapter::getValueRange() const
{
  if (!mTool)
    return QStringList();
  return mTool->getUSSectorConfigList();
}
QString ActiveToolConfigurationStringDataAdapter::convertInternal2Display(QString internal)
{
  if (!mTool)
    return "<no tool>";
  return mTool->getNameOfProbeSectorConfigId(internal); ///< get a name for the given configuration
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

  
ToolPropertiesWidget::ToolPropertiesWidget(QWidget* parent) :
  QWidget(parent)
{
  this->setObjectName("ToolPropertiesWidget");
  this->setWindowTitle("Tool Properties");

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

void ToolPropertiesWidget::dominantToolChangedSlot()
{
  ToolPtr cxTool = boost::shared_dynamic_cast<Tool>(mActiveTool);


  if (mActiveTool)
    disconnect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));
//  if (cxTool)
//    disconnect(cxTool.get(), SIGNAL(probeSectorConfigurationChanged()), this, SLOT(toolsSectorConfigurationChangedSlot()));

  mActiveTool = ssc::toolManager()->getDominantTool();

  if(mActiveTool->getType() == ssc::Tool::TOOL_US_PROBE)
  {
//    if (cxTool)
//      std::cout << "config: " << cxTool->getProbeSectorConfigurationString() << std::endl;
//    std::cout << "width: " << mActiveTool->getProbeSector().mWidth << std::endl;
//    std::cout << "dstart: " << mActiveTool->getProbeSector().mDepthStart<< std::endl;
//    std::cout << "dend: " << mActiveTool->getProbeSector().mDepthEnd << std::endl;
//    std::cout << "type: " << mActiveTool->getProbeSector().mType << std::endl;
//    std::cout << "w: " << mActiveTool->getProbeSector().mImage.mSize.width() << std::endl;
//    std::cout << "h: " << mActiveTool->getProbeSector().mImage.mSize.height() << std::endl;
//    std::cout << "c: " << mActiveTool->getProbeSector().mImage.mOrigin_u << std::endl;
//    std::cout << "s: " << mActiveTool->getProbeSector().mImage.mSpacing << std::endl;

    //    this->populateUSSectorConfigBox();
//    mUSSectorConfigLabel->show();
    mUSSectorConfigBox->show();
    mToptopLayout->update();
  }
  else
  {
//    mUSSectorConfigBox->clear();
//    mUSSectorConfigLabel->hide();
    mUSSectorConfigBox->hide();
    mToptopLayout->update();
  }

  if (mActiveTool)
    connect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));
//  if (cxTool)
//    connect(cxTool.get(), SIGNAL(probeSectorConfigurationChanged()), this, SLOT(toolsSectorConfigurationChangedSlot()));
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

//void ToolPropertiesWidget::populateUSSectorConfigBox()
//{
//  ToolPtr tool = boost::shared_dynamic_cast<Tool>(mActiveTool);
//
//  mUSSectorConfigBox->blockSignals(true);
//  mUSSectorConfigBox->clear();
//  mUSSectorConfigBox->insertItems(0, tool->getUSSectorConfigList());
//  this->toolsSectorConfigurationChangedSlot();//Read the tool's value into the combo box
//  mUSSectorConfigBox->blockSignals(false);
//}

//void ToolPropertiesWidget::configurationChangedSlot(int index)
//{
//  if(mActiveTool->getType() != ssc::Tool::TOOL_US_PROBE)//Only draw sectors for tools defined as US probes
//    return;
//
//  ToolPtr tool = boost::shared_dynamic_cast<Tool>(mActiveTool);
//  tool->setProbeSectorConfigurationString(mUSSectorConfigBox->currentText());
//}

//void ToolPropertiesWidget::toolsSectorConfigurationChangedSlot()
//{
//  ToolPtr tool = boost::shared_dynamic_cast<Tool>(mActiveTool);
//  //Only set tool's configurationString if tool don't have one
//  int index = 0;
//  if (!tool->getProbeSectorConfigurationString().isEmpty())
//  {
//    index = mUSSectorConfigBox->findText(tool->getProbeSectorConfigurationString());
//    if (index != -1)
//      mUSSectorConfigBox->setCurrentIndex(index);
//  }
//  // Can't use tool's current value if index is -1. Use default instead
//  if (tool->getProbeSectorConfigurationString().isEmpty() || (index == -1))
//    tool->setProbeSectorConfigurationString(mUSSectorConfigBox->currentText());
//}

void ToolPropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ToolPropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx
