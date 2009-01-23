#include "cxCustomStatusBar.h"

#include <QLabel>
#include <QString>
#include "cxToolManager.h"
#include <QHBoxLayout>
#include "cxMessageManager.h"
#include <QPixmap>
#include <QMetaObject>
/*
 * cxCustomStatusBar.cpp
 *
 *  Created on: Jan 21, 2009
 *      Author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
CustomStatusBar::CustomStatusBar() :
  mMessageManager(MessageManager::getInstance()),
  mToolManager(ToolManager::getInstance())
{
  connect(&mMessageManager, SIGNAL(emittedMessage(const QString&, int)),
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

    //TODO Add a pixelmap and name
    QPixmap pixmap;
    pixmap.fill(Qt::gray);
    QString toolName = QString(tool->getName().c_str());
    QLabel* label = new QLabel();
    label->setPixmap(pixmap);
    label->setText(toolName);
    //mToolColorMap->insert(std::pair<std::string, QPixmap*>(toolName, pixmap));
    mToolLabelVector.push_back(label);

    it++;
  }
}
void CustomStatusBar::disconnectFromToolSignals()
{
  ssc::ToolManager::ToolMapPtr connectedTools = mToolManager->getTools();
  ssc::ToolManager::ToolMap::iterator it = connectedTools->begin();
  while (it != connectedTools->end())
  {
    disconnect(it->second.get(), SIGNAL(toolVisible(bool)),
            this, SLOT(receiveToolVisible(bool)));
    //TODO Remove pixelmap and name
    it++;
  }
}
void CustomStatusBar::receiveToolVisible(bool visible)
{
  QObject* sender = this->sender();
  if(sender == 0)
  {
    mMessageManager.sendWarning("Could not determine which tool changed visibility.");
    return;
  }
  const QMetaObject* metaObject = sender->metaObject();
  const char* className = metaObject->className();
  std::cout << "Incoming objects classname is: " << className << std::endl;
  if(className == "Tool")
  {

  }

}
}//namespace cx
