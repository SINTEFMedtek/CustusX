#include <cxToolManagerWidget.h>

#include <QGridLayout>
#include <QPushButton>

#include "cxToolManager.h"

namespace cx
{

ToolManagerWidget::ToolManagerWidget(QWidget* parent) :
    BaseWidget(parent, "ToolManagerWidget", "ToolManager debugger"),
    mConfigureButton(new QPushButton("Configure")),
    mDeConfigureButton(new QPushButton("Deconfigure")),
    mInitializeButton(new QPushButton("Initialize")),
    mUnInitializeButton(new QPushButton("Uninitialize")),
    mStartTrackingButton(new QPushButton("Start Tracking")),
    mStopTrackingButton(new QPushButton("Stop Tracking"))
{
  //connect
  connect(mConfigureButton, SIGNAL(clicked(bool)), this, SLOT(configureClickedSlot(bool)));
  connect(mDeConfigureButton, SIGNAL(clicked(bool)), this, SLOT(deconfigureClickedSlot(bool)));
  connect(mInitializeButton, SIGNAL(clicked(bool)), this, SLOT(initializeClickedSlot(bool)));
  connect(mUnInitializeButton, SIGNAL(clicked(bool)), this, SLOT(uninitializeClickedSlot(bool)));
  connect(mStartTrackingButton, SIGNAL(clicked(bool)), this, SLOT(startTrackingClickedSlot(bool)));
  connect(mStopTrackingButton, SIGNAL(clicked(bool)), this, SLOT(stopTrackingClickedSlot(bool)));

  ToolManager* toolManager = ToolManager::getInstance();
  connect(toolManager, SIGNAL(configured()), this, SLOT(updateButtonStatusSlot()));
  connect(toolManager, SIGNAL(deconfigured()), this, SLOT(updateButtonStatusSlot()));
  connect(toolManager, SIGNAL(initialized()), this, SLOT(updateButtonStatusSlot()));
  connect(toolManager, SIGNAL(uninitialized()), this, SLOT(updateButtonStatusSlot()));
  connect(toolManager, SIGNAL(trackingStarted()), this, SLOT(updateButtonStatusSlot()));
  connect(toolManager, SIGNAL(trackingStopped()), this, SLOT(updateButtonStatusSlot()));

  //layout
  QGridLayout* layout = new QGridLayout(this);
  layout->addWidget(mConfigureButton, 0, 0);
  layout->addWidget(mDeConfigureButton, 0, 1);
  layout->addWidget(mInitializeButton, 1, 0);
  layout->addWidget(mUnInitializeButton, 1, 1);
  layout->addWidget(mStartTrackingButton, 2, 0);
  layout->addWidget(mStopTrackingButton, 2, 1);

  this->updateButtonStatusSlot();
}

ToolManagerWidget::~ToolManagerWidget()
{
}

QString ToolManagerWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>ToolManager debugging utilities.</h3>"
      "<p>Lets you test different aspects of the toolmanager.</p>"
      "<p><i></i></p>"
      "</html>";
}

void ToolManagerWidget::configureClickedSlot(bool checked)
{
  ToolManager::getInstance()->configure();
}

void ToolManagerWidget::deconfigureClickedSlot(bool checked)
{
  ToolManager::getInstance()->deconfigure();
}

void ToolManagerWidget::initializeClickedSlot(bool checked)
{
  ToolManager::getInstance()->initialize();
}

void ToolManagerWidget::uninitializeClickedSlot(bool checked)
{
  ToolManager::getInstance()->uninitialize();
}

void ToolManagerWidget::startTrackingClickedSlot(bool checked)
{
  ToolManager::getInstance()->startTracking();
}

void ToolManagerWidget::stopTrackingClickedSlot(bool checked)
{
  ToolManager::getInstance()->stopTracking();
}

void ToolManagerWidget::updateButtonStatusSlot()
{
  mConfigureButton->setDisabled(ToolManager::getInstance()->isConfigured());
  mDeConfigureButton->setDisabled(!ToolManager::getInstance()->isConfigured());
  mInitializeButton->setDisabled(ToolManager::getInstance()->isInitialized());
  mUnInitializeButton->setDisabled(!ToolManager::getInstance()->isInitialized());
  mStartTrackingButton->setDisabled(ToolManager::getInstance()->isTracking());
  mStopTrackingButton->setDisabled(!ToolManager::getInstance()->isTracking());
}
}
