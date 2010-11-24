/*
 * cxFrameTreeWidget.cpp
 *
 *  Created on: Sep 23, 2010
 *      Author: christiana
 */

#include "cxFrameTreeWidget.h"

#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "cxFrameForest.h"

namespace cx
{


FrameTreeWidget::FrameTreeWidget(QWidget* parent) :
  QWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("FrameTreeWidget");
  this->setWindowTitle("Frame Tree");

  //layout->setMargin(0);

  mTreeWidget = new QTreeWidget(this);
  layout->addWidget(mTreeWidget);
  mTreeWidget->setHeaderLabels(QStringList() << "Frame");

  // TODO this must also listen to all changed() in all data
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataLoadedSlot()));
}

void FrameTreeWidget::dataLoadedSlot()
{
  for (ssc::DataManager::DataMap::iterator iter=mConnectedData.begin(); iter!=mConnectedData.end(); ++iter)
  {
    disconnect(iter->second.get(), SIGNAL(transformChanged()), this, SLOT(rebuild()));
  }

  mConnectedData = ssc::dataManager()->getData();

  for (ssc::DataManager::DataMap::iterator iter=mConnectedData.begin(); iter!=mConnectedData.end(); ++iter)
  {
    connect(iter->second.get(), SIGNAL(transformChanged()), this, SLOT(rebuild()));
  }

  this->rebuild();
}

void FrameTreeWidget::rebuild()
{
  mTreeWidget->clear();

  FrameForest forest;
  QDomElement root = forest.getDocument().documentElement();

  this->fill(mTreeWidget->invisibleRootItem(), root);

//  for (QDomNode child = root.firstChild(); !child.isNull(); child = child.nextSibling())
//  {
//    QTreeWidgetItem* item = new QTreeWidgetItem(mTreeWidget, QStringList() << child.toElement().tagName());
//    this->fill(item, child);
//  }

  mTreeWidget->expandToDepth(10);
  mTreeWidget->resizeColumnToContents(0);
}

void FrameTreeWidget::fill(QTreeWidgetItem* parent, QDomNode node)
{
  for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling())
  {
    QString frameName = child.toElement().tagName();

    // if frame refers to a data, use its name instead.
    ssc::DataPtr data = ssc::dataManager()->getData(frameName);
    if (data)
      frameName = data->getName();

    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList() << frameName);
    this->fill(item, child);
  }
}

}
