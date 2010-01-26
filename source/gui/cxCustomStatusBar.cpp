#include "cxCustomStatusBar.h"

#include <QLabel>
#include <QString>
#include "cxToolManager.h"
#include <QHBoxLayout>
#include "cxMessageManager.h"
#include <QPixmap>
#include <QMetaObject>

namespace cx
{
CustomStatusBar::CustomStatusBar() :
  mMessageManager(MessageManager::getInstance()),
  mToolManager(ToolManager::getInstance())
{
  connect(mMessageManager, SIGNAL(emittedMessage(const QString&, int)),
          this, SLOT(showMessage(const QString&, int)));

  connect(mToolManager, SIGNAL(trackingStarted()),
          this, SLOT(connectToToolSignals()));
  connect(mToolManager, SIGNAL(trackingStopped()),
            this, SLOT(disconnectFromToolSignals()));
}
CustomStatusBar::~CustomStatusBar()
{
}
void CustomStatusBar::connectToToolSignals()
{
  ssc::ToolManager::ToolMapPtr connectedTools = mToolManager->getTools();
  ssc::ToolManager::ToolMap::iterator it = connectedTools->begin();
  while (it != connectedTools->end())
  {
    ssc::Tool* tool = it->second.get();
    connect(tool, SIGNAL(toolVisible(bool)),
            this, SLOT(receiveToolVisible(bool)));

    QPixmap pixmap;
    pixmap.fill(Qt::gray);
    QString toolName = QString(tool->getName().c_str());

    QLabel* textLabel = new QLabel();
    textLabel->setText(toolName);

    QLabel* colorLabel = new QLabel();
    colorLabel->setPixmap(pixmap);

    mToolColorMap.insert(std::pair<QLabel*,QLabel*>(textLabel, colorLabel));

    it++;
  }
}
void CustomStatusBar::disconnectFromToolSignals()
{
  ssc::ToolManager::ToolMapPtr connectedTools = mToolManager->getTools();
  ssc::ToolManager::ToolMap::iterator it1 = connectedTools->begin();
  while (it1 != connectedTools->end())
  {
    disconnect(it1->second.get(), SIGNAL(toolVisible(bool)),
            this, SLOT(receiveToolVisible(bool)));
    it1++;
  }
  std::cout << "mToolColorMap.size() = " << mToolColorMap.size() << std::endl;
  std::map<QLabel*, QLabel*>::iterator it2 = mToolColorMap.begin();
  while(it2 != mToolColorMap.end())
  {
    QLabel* textLabel = it2->first;
    QLabel* colorLabel = it2->second;
    this->removeWidget(textLabel);
    this->removeWidget(colorLabel);
    mToolColorMap.erase(it2);

    //RemoveWidget does not delete the widget but hides it...
    delete textLabel;
    delete colorLabel;

    it2++;
  }
}
void CustomStatusBar::receiveToolVisible(bool visible)
{
  QObject* sender = this->sender();
  if(sender == 0)
  {
    mMessageManager->sendWarning("Could not determine which tool changed visibility.");
    return;
  }
  const QMetaObject* metaObject = sender->metaObject();
  std::string className = metaObject->className();
  std::cout << "Incoming objects classname is: " << className << std::endl; //TODO debuggging
  if(className == "Tool")
  {
    Tool* tool = dynamic_cast<Tool*>(sender);
    if(tool == NULL)
    {
      mMessageManager->sendWarning("The sender does not appear to be a tool.");
      return;
    }
    std::string name = tool->getName();
    std::map<QLabel*, QLabel*>::iterator it = mToolColorMap.begin();
    while(it != mToolColorMap.end())
    {
      QLabel* textLabel = it->first;
      if(textLabel->text().compare(QString(name.c_str()), Qt::CaseInsensitive) == 0)
      {
        QLabel* colorLabel = it->second;
        QPixmap pixmap;
        if(visible)
        {
          pixmap.fill(Qt::green);
        }else
        {
          pixmap.fill(Qt::red);
        }
        colorLabel->setPixmap(pixmap);
        std::cout << "Set new pixmap. " << std::endl; //TODO debuggging
      }
      it++;
    }
  }

}
}//namespace cx
