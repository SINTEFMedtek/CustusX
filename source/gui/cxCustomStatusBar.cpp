#include "cxCustomStatusBar.h"

#include <QLabel>
#include <QString>
#include "sscToolManager.h"
#include <QHBoxLayout>
#include "sscMessageManager.h"
#include "cxViewManager.h"
#include <QPixmap>
#include <QMetaObject>

namespace cx
{
CustomStatusBar::CustomStatusBar() :
  mFpsLabel(new QLabel()),
  mTpsLabel(new QLabel())
{
  connect(ssc::messageManager(), SIGNAL(emittedMessage(Message)), this, SLOT(showMessageSlot(Message)));

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(connectToToolSignals()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(disconnectFromToolSignals()));
  connect(ssc::toolManager(), SIGNAL(tps(int)), this, SLOT(tpsSlot(int)));
  connect(viewManager(), SIGNAL(fps(int)),this, SLOT(fpsSlot(int)));
  
  this->addPermanentWidget(mFpsLabel);
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
    connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveToolVisible(bool)));

    QString toolName = tool->getName();

    QLabel* toolLabel = new QLabel(toolName);
    this->setToolLabelColor(toolLabel, tool->getVisible());
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
    disconnect(toolIt->second.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveToolVisible(bool)));
  }
  for(unsigned i=0; i<mToolLabels.size(); ++i)
  {
    QLabel* toolLabel = mToolLabels[i];
    this->removeWidget(toolLabel);
    delete toolLabel;
  }
  mToolLabels.clear();
}

void CustomStatusBar::receiveToolVisible(bool visible)
{
  ssc::Tool* tool = dynamic_cast<ssc::Tool*>(this->sender());
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
      this->setToolLabelColor(toolLabel, tool->getVisible());
  }
}

void CustomStatusBar::setToolLabelColor(QLabel* label, bool visible)
{
  QString color;
  if(visible)
    color = QString("QLabel { background-color: green }");
  else
    color = QString("QLabel { background-color: red }");

  label->setStyleSheet(color);
}

void CustomStatusBar::fpsSlot(int numFps)
{
  QString fpsString = "FPS: "+QString::number(numFps);
  mFpsLabel->setText(fpsString);
}

void CustomStatusBar::tpsSlot(int numTps)
{
  QString tpsString = "TPS: "+QString::number(numTps);
  mTpsLabel->setText(tpsString);
}

void CustomStatusBar::showMessageSlot(Message message)
{
  this->showMessage(message.getPrintableMessage(), message.getTimeout());
}

}//namespace cx
