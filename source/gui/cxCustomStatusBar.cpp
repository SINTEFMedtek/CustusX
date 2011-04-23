#include "cxCustomStatusBar.h"

#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "cxStateMachineManager.h"
#include "RTSource/cxRTSourceManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include <QPixmap>
#include <QMetaObject>

namespace cx
{
CustomStatusBar::CustomStatusBar() :
  mRenderingFpsLabel(new QLabel()),
  mGrabbingInfoLabel(new QLabel()),
  mTpsLabel(new QLabel())
{
  connect(ssc::messageManager(), SIGNAL(emittedMessage(Message)), this, SLOT(showMessageSlot(Message)));

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(connectToToolSignals()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(disconnectFromToolSignals()));
  connect(ssc::toolManager(), SIGNAL(tps(int)), this, SLOT(tpsSlot(int)));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(receiveToolDominant()));

  connect(viewManager(), SIGNAL(fps(int)),this, SLOT(renderingFpsSlot(int)));
  
  connect(stateManager()->getRTSourceManager().get(), SIGNAL(fps(int)), this, SLOT(grabbingFpsSlot(int)));
  connect(stateManager()->getRTSourceManager().get(), SIGNAL(connected(bool)), this, SLOT(grabberConnectedSlot(bool)));

  this->addPermanentWidget(mRenderingFpsLabel);
}

CustomStatusBar::~CustomStatusBar()
{}

void CustomStatusBar::connectToToolSignals()
{
  this->addPermanentWidget(mTpsLabel);

  ssc::ToolManager::ToolMapPtr initializedTools = ssc::toolManager()->getInitializedTools();
  ssc::ToolManager::ToolMap::iterator it = initializedTools->begin();
  for(;it != initializedTools->end(); ++it)
  {
    ssc::ToolPtr tool = it->second;
    if(tool->getType() == ssc::Tool::TOOL_MANUAL)
      continue;
    if(tool == ToolManager::getInstance()->getManualTool())
      continue;
    connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveToolVisible()));

    QString toolName = tool->getName();

    QLabel* toolLabel = new QLabel(toolName);
    this->setToolLabelColor(toolLabel, tool->getVisible(), ssc::toolManager()->getDominantTool()==tool);
    this->addPermanentWidget(toolLabel);
    mToolLabels.push_back(toolLabel);
  }
}

void CustomStatusBar::disconnectFromToolSignals()
{
  this->removeWidget(mTpsLabel);
  ssc::ToolManager::ToolMapPtr initializedTools = ssc::toolManager()->getInitializedTools();
  ssc::ToolManager::ToolMap::iterator toolIt = initializedTools->begin();
  for(;toolIt != initializedTools->end(); ++toolIt)
  {
    disconnect(toolIt->second.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveToolVisible()));
  }
  for(unsigned i=0; i<mToolLabels.size(); ++i)
  {
    QLabel* toolLabel = mToolLabels[i];
    this->removeWidget(toolLabel);
    delete toolLabel;
  }
  mToolLabels.clear();
}

void CustomStatusBar::receiveToolVisible()
{
  ssc::Tool* tool = dynamic_cast<ssc::Tool*>(this->sender());
  this->colorTool(tool);
}

void CustomStatusBar::receiveToolDominant()
{
  ssc::ToolManager::ToolMapPtr initializedTools = ssc::toolManager()->getInitializedTools();
  ssc::ToolManager::ToolMap::iterator it = initializedTools->begin();
  for(;it != initializedTools->end(); ++it)
  {
    ssc::ToolPtr tool = it->second;
    this->colorTool(tool.get());
  }
}

void CustomStatusBar::colorTool(ssc::Tool* tool)
{
  if(!tool)
  {
    ssc::messageManager()->sendWarning("Could not determine which tool changed visibility.");
    return;
  }

  QString name = tool->getName();
  for(unsigned i=0; i<mToolLabels.size(); ++i)
  {
    QLabel* toolLabel = mToolLabels[i];
    if(toolLabel->text().compare(name, Qt::CaseInsensitive) == 0)
      this->setToolLabelColor(toolLabel, tool->getVisible(), ssc::toolManager()->getDominantTool().get()==tool);
  }
}

void CustomStatusBar::setToolLabelColor(QLabel* label, bool visible, bool dominant)
{
  QString color;
  if(visible)
  {
    if (dominant)
      color = QString("QLabel { background-color: lime }");
    else
      color = QString("QLabel { background-color: green }");
  }
  else
    color = QString("QLabel { background-color: red }");

  label->setStyleSheet(color);
}

void CustomStatusBar::renderingFpsSlot(int numFps)
{
  QString fpsString = "FPS: "+QString::number(numFps);
  mRenderingFpsLabel->setText(fpsString);
}

void CustomStatusBar::tpsSlot(int numTps)
{
  QString tpsString = "TPS: "+QString::number(numTps);
  mTpsLabel->setText(tpsString);
}

void CustomStatusBar::grabbingFpsSlot(int numFps)
{
  ssc::OpenIGTLinkRTSourcePtr grabber = stateManager()->getRTSourceManager()->getRTSource();
  QString infoString = grabber->getName()+"-FPS: "+QString::number(numFps);
  mGrabbingInfoLabel->setText(infoString);
}

void CustomStatusBar::grabberConnectedSlot(bool connected)
{
  if(connected)
    this->addPermanentWidget(mGrabbingInfoLabel);
  else
    this->removeWidget(mGrabbingInfoLabel);
}

void CustomStatusBar::showMessageSlot(Message message)
{
  this->showMessage(message.getPrintableMessage(), message.getTimeout());
}

}//namespace cx
