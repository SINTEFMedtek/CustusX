#ifndef CXBROWSERWIDGET_H_
#define CXBROWSERWIDGET_H_

#include "cxBaseWidget.h"

#include <QtGui>
#include "cxImage.h"
#include "cxTreeModelItem.h"
#include "cxStringProperty.h"
#include "cxLabeledComboBoxWidget.h"
class QTreeWidget;
class QVBoxLayout;
class QTreeWidgetItem;
class QTreeView;

namespace cx
{

class TreeItemModel;

/**
 * \class BrowserWidget
 *
 * \brief Shows a treestructure containing the loaded images, meshes
 * and tools.
 * \ingroup cxGUI
 *
 * \date Feb 11, 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class BrowserWidget : public BaseWidget
{
  Q_OBJECT

public:
  BrowserWidget(QWidget* parent);
  ~BrowserWidget();

protected slots:
  void resetView(); // called when tree is reset

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void closeEvent(QCloseEvent* event); ///<disconnects stuff

  TreeItemModel* mModel;
  QTreeView* mTreeView;
  //gui
  //QTreeWidget* mTreeWidget; ///< the treestructure containing the images, meshes and tools
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used

protected:
	virtual void prePaintEvent();
  
private:
  BrowserWidget();
};
}//end namespace cx

#endif /* CXBROWSERWIDGET_H_ */
