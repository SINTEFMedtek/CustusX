#include "cxCustomStatusBar.h"

#include <QLabel>
#include <QString>
#include "cxToolManager.h"
#include <QHBoxLayout>
#include "cxMessageManager.h"
#include "cxViewManager.h"
#include <QPixmap>
#include <QMetaObject>

namespace cx
{
CustomStatusBar::CustomStatusBar() :
  mFpsLabel(new QLabel())
{
  connect(messageManager(),
          SIGNAL(emittedMessage(const QString&, int)),
          this,
          SLOT(showMessage(const QString&, int)));

  connect(ToolManager::getInstance(), SIGNAL(trackingStarted()),
          this, SLOT(connectToToolSignals()));
  connect(ToolManager::getInstance(), SIGNAL(trackingStopped()),
            this, SLOT(disconnectFromToolSignals()));
  connect(ViewManager::getInstance(), SIGNAL(fps(int)),
          this, SLOT(fpsSlot(int)));
  
  this->addPermanentWidget(mFpsLabel);
}

CustomStatusBar::~CustomStatusBar()
{}

void CustomStatusBar::connectToToolSignals()
{
  ssc::ToolManager::ToolMapPtr connectedTools = toolManager()->getTools();
  ssc::ToolManager::ToolMap::iterator it = connectedTools->begin();
  while (it != connectedTools->end())
  {
    ssc::Tool* tool = it->second.get();
    connect(tool, SIGNAL(toolVisible(bool)), this, SLOT(receiveToolVisible(bool)));

    //QPixmap pixmap;
    //pixmap.fill(tool->getVisible() ? Qt::green : Qt::red);
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

    //std::cout << "Added permanent label with name " << toolLabel->text().toStdString() << std::endl;
    std::cout << "Added permanent label with name " << toolName.toStdString() << std::endl;

    it++;
  }
}

void CustomStatusBar::disconnectFromToolSignals()
{
  ssc::ToolManager::ToolMapPtr connectedTools = ToolManager::getInstance()->getTools();
  ssc::ToolManager::ToolMap::iterator toolIt = connectedTools->begin();
  while (toolIt != connectedTools->end())
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
  Tool* tool = dynamic_cast<Tool*>(this->sender());
  if(!tool)
  {
    MessageManager::getInstance()->sendWarning("Could not determine which tool changed visibility.");
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
      std::cout << "Set new pixmap for tool: "<< tool->getName() << std::endl; //TODO debuggging
    }
  }
}

void CustomStatusBar::fpsSlot(int numFps)
{
  QString fpsString = "FPS: "+QString::number(numFps);
  mFpsLabel->setText(fpsString);
}

}//namespace cx
