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
  mFpsLabel(new QLabel())
{
  connect(ssc::messageManager(),
          SIGNAL(emittedMessage(const QString&, int)),
          this,
          SLOT(showMessage(const QString&, int)));

  connect(ssc::toolManager(), SIGNAL(trackingStarted()),
          this, SLOT(connectToToolSignals()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()),
            this, SLOT(disconnectFromToolSignals()));
  connect(viewManager(), SIGNAL(fps(int)),
          this, SLOT(fpsSlot(int)));
  
  this->addPermanentWidget(mFpsLabel);
}

CustomStatusBar::~CustomStatusBar()
{}

void CustomStatusBar::connectToToolSignals()
{
  ssc::ToolManager::ToolMapPtr initializedTools = ssc::toolManager()->getInitializedTools();
  ssc::ToolManager::ToolMap::iterator it = initializedTools->begin();
  while (it != initializedTools->end())
  {
    ssc::Tool* tool = it->second.get();
    if(tool->getType() == ssc::Tool::TOOL_MANUAL)
    {
      it++;
      continue;
    }
    connect(tool, SIGNAL(toolVisible(bool)), this, SLOT(receiveToolVisible(bool)));

    QString toolName = QString(tool->getName().c_str());

    QLabel* toolLabel = new QLabel();
    toolLabel->setText(toolName);

    QString color;
    if(tool->getVisible())
      color = QString("QLabel { background-color: green }");
    else
      color = QString("QLabel { background-color: red }");
    toolLabel->setStyleSheet(color);

    this->addPermanentWidget(toolLabel);
    mToolLabels.push_back(toolLabel);
    //std::cout << "Added permanent label with name " << toolName.toStdString() << std::endl;
    it++;
  }
}

void CustomStatusBar::disconnectFromToolSignals()
{
  ssc::ToolManager::ToolMapPtr initializedTools = ssc::toolManager()->getInitializedTools();
  ssc::ToolManager::ToolMap::iterator toolIt = initializedTools->begin();
  while (toolIt != initializedTools->end())
  {
    disconnect(toolIt->second.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveToolVisible(bool)));
    toolIt++;
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

  std::string name = tool->getName();
  for(unsigned i=0; i<mToolLabels.size(); ++i)
  {
    QLabel* toolLabel = mToolLabels[i];
    if(toolLabel->text().compare(QString(name.c_str()), Qt::CaseInsensitive) == 0)
    {
      QString color;
      if(tool->getVisible())
        color = QString("QLabel { background-color: green }");
      else
        color = QString("QLabel { background-color: red }");

      toolLabel->setStyleSheet(color);
    }
  }
}

void CustomStatusBar::fpsSlot(int numFps)
{
  QString fpsString = "FPS: "+QString::number(numFps);
  mFpsLabel->setText(fpsString);
}

}//namespace cx
