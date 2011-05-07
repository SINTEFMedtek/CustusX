#ifndef CXFRAMETREEWIDGET_H_
#define CXFRAMETREEWIDGET_H_

#include <map>
#include <string>
#include <QWidget>
#include "sscForwardDeclarations.h"

class QTreeWidget;
class QTreeWidgetItem;
class QDomNode;

namespace cx
{

/**
 * \class FrameTreeWidget
 *
 *\brief Widget for displaying the FrameForest object
 *
 *\date Sep 23, 2010
 *\author: christiana
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
  std::map<QString, ssc::DataPtr> mConnectedData;

private slots:
  void dataLoadedSlot();
  void rebuild(); // TODO this must also listen to all changed() in all data
};


}

#endif /* CXFRAMETREEWIDGET_H_ */
