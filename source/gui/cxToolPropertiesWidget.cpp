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
#include "sscImage.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "sscToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "sscImageLUT2D.h"
#include "cxDataInterface.h"
#include "UsConfigGui.h"

namespace cx
{


ToolPropertiesWidget::ToolPropertiesWidget(QWidget* parent) :
  QWidget(parent),
  mProbePropertiesWidget(new UsConfigGui(this))
{
  this->setObjectName("ToolPropertiesWidget");
  this->setWindowTitle("Tool Properties");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  QHBoxLayout* generalLayout = new QHBoxLayout;
  mReferenceStatusLabel = new QLabel("Reference frame <undefined>", this);
  generalLayout->addWidget(mReferenceStatusLabel);
  mTrackingSystemStatusLabel = new QLabel("Tracking <undefined>", this);
  generalLayout->addWidget(mTrackingSystemStatusLabel);

  toptopLayout->addLayout(generalLayout);

  QGroupBox* activeGroup = new QGroupBox(this);
  activeGroup->setTitle("Active Tool");
  toptopLayout->addWidget(activeGroup);
  QVBoxLayout* activeGroupLayout = new QVBoxLayout;
  activeGroup->setLayout(activeGroupLayout);

  QHBoxLayout* activeToolLayout = new QHBoxLayout;
  activeToolLayout->addWidget(new QLabel("Name:", this));
  mToolNameLabel = new QLabel(this);
  activeToolLayout->addWidget(mToolNameLabel);
  mActiveToolVisibleLabel = new QLabel("Visible: NA");
  activeToolLayout->addWidget(mActiveToolVisibleLabel);
  activeGroupLayout->addLayout(activeToolLayout);
  
  toptopLayout->addWidget(mProbePropertiesWidget);

//  QGroupBox* group2D = new QGroupBox(this);
//  group2D->setTitle("2D properties");
//  toptopLayout->addWidget(group2D);

  QGridLayout* gridLayout = new QGridLayout;
  activeGroupLayout->addLayout(gridLayout);

  mToolOffsetWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterActiveToolOffset), gridLayout, 0);

  toptopLayout->addStretch();

//  void configured(); ///< signal emitted when the system is configured
//  void initialized(); ///< signal emitted when the system is initialized
//  void trackingStarted(); ///< signal emitted when the system starts tracking
//  void trackingStopped(); ///< signal emitted when the system stops tracking

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(referenceToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(referenceToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));

  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(updateSlot()));

  dominantToolChangedSlot();
  referenceToolChangedSlot();
  updateSlot();
}

ToolPropertiesWidget::~ToolPropertiesWidget()
{
}

void ToolPropertiesWidget::dominantToolChangedSlot()
{
  if (mActiveTool)
    disconnect(mActiveTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateSlot()));

  mActiveTool = ssc::toolManager()->getDominantTool();

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
