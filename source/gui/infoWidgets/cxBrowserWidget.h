/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXBROWSERWIDGET_H_
#define CXBROWSERWIDGET_H_

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxPopupToolbarWidget.h"
#include "cxXmlOptionItem.h"
#include "cxControllableSplitter.h"
#include "cxReplacableContentWidget.h"

class QTreeWidget;
class QVBoxLayout;
class QTreeWidgetItem;
class QTreeView;
class QSplitter;
class QLineEdit;

namespace cx
{

class PopupToolbarWidget;
class TreeItemModel;
class EraseDataToolButton;
typedef boost::shared_ptr<class TreeNode> TreeNodePtr;
typedef boost::shared_ptr<class TreeRepository> TreeRepositoryPtr;


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
  BrowserWidget(QWidget* parent, VisServicesPtr services);
  ~BrowserWidget();

protected slots:
  void resetView(); // called when tree is reset

protected:
  TreeItemModel* mModel;
  QPointer<QTreeView> mTreeView;

protected:
	virtual void prePaintEvent();
  
private:
  void createGUI();
  void onPopup();
  void onCurrentItemChanged();
  void createButtonWidget(QWidget* widget);
  XmlOptionItem getShowToolbarOption();
  void expandDefault(QModelIndex index);
  void updateNodeName();
  void eraseCurrentNode();
  void onLoaded();
  void onNodeCollapsed(const QModelIndex & index);
  void onNodeExpanded(const QModelIndex & index);
  void expandRestore(QModelIndex index);
  bool setCurrentNode(QString uid, QModelIndex index);

  VisServicesPtr mServices;
  ControllableSplitter* mSplitter;
  QPointer<ReplacableContentWidget> mPropertiesWidget;
  PopupToolbarWidget* mPopupWidget;
  XmlOptionFile mOptions;
  QLineEdit* mName;
  EraseDataToolButton* mRemoveButton;

  StringPropertyPtr mFilterSelector;
  QList<QString> mExpanded;
  QString mActiveNodeUid;
};
}//end namespace cx

#endif /* CXBROWSERWIDGET_H_ */
