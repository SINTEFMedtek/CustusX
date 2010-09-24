/*
 * cxFrameTreeWidget.h
 *
 *  Created on: Sep 23, 2010
 *      Author: christiana
 */

#ifndef CXFRAMETREEWIDGET_H_
#define CXFRAMETREEWIDGET_H_

#include <map>
#include <string>
#include <QWidget>
class QTreeWidget;
class QTreeWidgetItem;
class QDomNode;
#include "sscForwardDeclarations.h"

namespace cx
{

/** Widget for displaying the FrameForest object
 *
 */
class FrameTreeWidget : public QWidget
{
  Q_OBJECT
public:
  FrameTreeWidget(QWidget* parent);
  ~FrameTreeWidget() {}
private:
  QTreeWidget* mTreeWidget;
  void fill(QTreeWidgetItem* parent, QDomNode node);
  std::map<std::string, ssc::DataPtr> mConnectedData;
private slots:
  void dataLoadedSlot();
  void rebuild(); // TODO this must also listen to all changed() in all data
};


}

#endif /* CXFRAMETREEWIDGET_H_ */
