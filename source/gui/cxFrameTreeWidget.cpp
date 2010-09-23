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
  this->setWindowTitle("Frame Tree Widget");

  //layout->setMargin(0);

  mTreeWidget = new QTreeWidget(this);
  layout->addWidget(mTreeWidget);

  // TODO this must also listen to all changed() in all data
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(rebuild()));
}

void FrameTreeWidget::rebuild()
{
  mTreeWidget->clear();
  mTreeWidget->setHeaderLabels(QStringList() << "Frame");

  FrameForest forest;
  QDomElement root = forest.getDocument().documentElement();

//  QTreeWidgetItem* rootItem = new QTreeWidgetItem(mTreeWidget, QStringList() << "root");
//  this->fill(rootItem, root);

  for (QDomNode child = root.firstChild(); !child.isNull(); child = child.nextSibling())
  {
    QTreeWidgetItem* item = new QTreeWidgetItem(mTreeWidget, QStringList() << child.toElement().tagName());
    this->fill(item, child);
  }

  mTreeWidget->expandToDepth(5);
  mTreeWidget->resizeColumnToContents(0);
  //mTreeWidget->setRootIsDecorated(false);
}

void FrameTreeWidget::fill(QTreeWidgetItem* parent, QDomNode node)
{
  for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling())
  {
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList() << child.toElement().tagName());
    this->fill(item, child);
  }
}

}
