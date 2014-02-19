#ifndef CXFRAMETREEWIDGET_H_
#define CXFRAMETREEWIDGET_H_

#include "cxBaseWidget.h"

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
 * \ingroup cxGUI
 *
 *\date Sep 23, 2010
 *\\author Christian Askeland, SINTEF
 */
class FrameTreeWidget : public BaseWidget
{
  Q_OBJECT
public:
  FrameTreeWidget(QWidget* parent);
  ~FrameTreeWidget() {}

  virtual QString defaultWhatsThis() const;

protected:
  virtual void prePaintEvent();
private:
  QTreeWidget* mTreeWidget;
  void fill(QTreeWidgetItem* parent, QDomNode node);
  std::map<QString, DataPtr> mConnectedData;

private slots:
  void dataLoadedSlot();
  void rebuild(); // TODO this must also listen to all changed() in all data
};


}

#endif /* CXFRAMETREEWIDGET_H_ */
