#include <cxToolFilterWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include "sscEnumConverter.h"
#include "cxStateService.h"
#include "cxToolManager.h"
#include "cxSelectionGroupBox.h"
#include "cxToolListWidget.h"
#include "cxApplicationStateMachine.h"

namespace cx
{

ToolFilterGroupBox::ToolFilterGroupBox(QWidget* parent) :
    QGroupBox(parent)
{
  this->setTitle("Tool filter");

  mApplicationGroupBox = new SelectionGroupBox("Applications", stateService()->getApplication()->getAllApplicationNames(), false, NULL);
  mApplicationGroupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);
  mTrackingSystemGroupBox = new SelectionGroupBox("Tracking systems", ToolManager::getInstance()->getSupportedTrackingSystems(), true, NULL);
  mToolListWidget = new FilteringToolListWidget(NULL);
  connect(mToolListWidget, SIGNAL(toolSelected(QString)), this, SIGNAL(toolSelected(QString)));

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

void ToolFilterGroupBox::setClinicalApplicationSlot(ssc::CLINICAL_APPLICATION clinicalApplication)
{
  QStringList selectedApplication;
  selectedApplication << enum2string(clinicalApplication);
  mApplicationGroupBox->setSelected(selectedApplication);
}

void ToolFilterGroupBox::setTrackingSystemSlot(ssc::TRACKING_SYSTEM trackingSystem)
{
  QStringList selectedTrackingSystem;
  selectedTrackingSystem << enum2string(trackingSystem);
  mTrackingSystemGroupBox->setSelected(selectedTrackingSystem);
}

void ToolFilterGroupBox::filterSlot()
{
  QStringList applicationFilter = mApplicationGroupBox->getSelected();
  QStringList trackingSystemFilter = mTrackingSystemGroupBox->getSelected();

  mToolListWidget->filterSlot(applicationFilter, trackingSystemFilter);
}
}//namespace cx
