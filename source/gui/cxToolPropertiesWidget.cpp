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
#include "sscTypeConversions.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "sscImageLUT2D.h"
#include "cxDataInterface.h"

namespace cx
{


ToolPropertiesWidget::ToolPropertiesWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("ToolPropertiesWidget");
  this->setWindowTitle("ToolProperties");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  mToolNameLabel = new QLabel(this);
  toptopLayout->addWidget(mToolNameLabel);

//  QGroupBox* group2D = new QGroupBox(this);
//  group2D->setTitle("2D properties");
//  toptopLayout->addWidget(group2D);

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);

  mToolOffsetWidget = new SliderGroupWidget(this, ssc::DoubleDataInterfacePtr(new DoubleDataInterfaceActiveToolOffset), gridLayout, 0);

  connect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(updateSlot()));
  updateSlot();
}

ToolPropertiesWidget::~ToolPropertiesWidget()
{
}

void ToolPropertiesWidget::updateSlot()
{
  ssc::ToolPtr tool = toolManager()->getDominantTool();
  if (tool)
  {
    mToolNameLabel->setText(qstring_cast(tool->getName()));
  }
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
