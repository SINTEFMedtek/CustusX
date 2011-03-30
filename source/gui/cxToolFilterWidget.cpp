#include <cxToolFilterWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include "cxStateMachineManager.h"
#include "cxToolManager.h"
#include "cxSelectionGroupBox.h"
#include "cxToolListWidget.h"

namespace cx
{

ToolFilterGroupBox::ToolFilterGroupBox(QWidget* parent) :
    QGroupBox(parent)
{
  this->setTitle("Tool filter");

  mApplicationGroupBox = new SelectionGroupBox("Applications", stateManager()->getApplication()->getAllApplicationNames(), false, NULL);
  mTrackingSystemGroupBox = new SelectionGroupBox("Tracking stystem", ToolManager::getInstance()->getSupportedTrackingSystems(), true, NULL);
  mToolListWidget = new FilteringToolListWidget(NULL);

  QGridLayout* layout = new QGridLayout(this);
  layout->addWidget(mApplicationGroupBox);
  connect(mApplicationGroupBox, SIGNAL(selectionChanged()), this, SLOT(filterSlot()));
  layout->addWidget(mTrackingSystemGroupBox);
  connect(mTrackingSystemGroupBox, SIGNAL(selectionChanged()), this, SLOT(filterSlot()));

  QGroupBox* toolGroupBox = new QGroupBox();
  toolGroupBox->setTitle("Tools");
  QVBoxLayout* toolLayout = new QVBoxLayout();
  toolGroupBox->setLayout(toolLayout);
  toolLayout->addWidget(mToolListWidget);

  layout->addWidget(toolGroupBox);
}

ToolFilterGroupBox::~ToolFilterGroupBox()
{}

void ToolFilterGroupBox::filterSlot()
{
  QStringList applicationFilter = mApplicationGroupBox->getSelected();
  QStringList trackingSystemFilter = mTrackingSystemGroupBox->getSelected();

  mToolListWidget->filterSlot(applicationFilter, trackingSystemFilter);
}
}//namespace cx
