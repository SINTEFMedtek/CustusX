#ifndef CXBROWSERWIDGET_H_
#define CXBROWSERWIDGET_H_

#include <QWidget>
#include <QtGui>
#include "sscImage.h"
#include "cxTreeModelItem.h"
#include "sscStringDataAdapterXml.h"
#include "sscStringWidgets.h"
class QTreeWidget;
class QVBoxLayout;
class QTreeWidgetItem;

namespace cx
{

class TreeItem;
typedef boost::weak_ptr<TreeItem> TreeItemWeakPtr;
typedef boost::shared_ptr<TreeItem> TreeItemPtr;

class BrowserItemModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  BrowserItemModel(QObject* parent = 0);
  virtual ~BrowserItemModel();
  void setSelectionModel(QItemSelectionModel* selectionModel);

  virtual int columnCount (const QModelIndex& parent = QModelIndex() ) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
  virtual Qt::ItemFlags flags(const QModelIndex& index ) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex& index ) const;

  //  virtual bool hasChildren(const QModelIndex& parent = QModelIndex() ) const;
  //  virtual bool    setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
  ssc::StringDataAdapterXmlPtr getFilter();

signals:
  void hasBeenReset();

public slots:
  void buildTree();
  void treeItemChangedSlot();

private slots:
  void currentItemChangedSlot(const QModelIndex& current, const QModelIndex& previous);

private:
  TreeItemPtr mTree;
  QItemSelectionModel* mSelectionModel;
  ssc::StringDataAdapterXmlPtr mFilter;
  TreeItem* itemFromIndex(const QModelIndex& index) const;
};

/**
 * \class BrowserWidget
 *
 * \brief Shows a treestructure containing the loaded images, meshes
 * and tools
 *
 * \date Feb 11, 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class BrowserWidget : public QWidget
{
  Q_OBJECT

public:
  BrowserWidget(QWidget* parent);
  ~BrowserWidget();

protected slots:
  void populateTreeWidget(); ///< fills the tree
  void resetView(); // called when tree is reset

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void closeEvent(QCloseEvent* event); ///<disconnects stuff

  BrowserItemModel* mModel;
  QTreeView* mTreeView;
  //gui
  //QTreeWidget* mTreeWidget; ///< the treestructure containing the images, meshes and tools
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used

private:
  BrowserWidget();
};
}//end namespace cx

#endif /* CXBROWSERWIDGET_H_ */
