#ifndef CXTREEITEMMODEL_H
#define CXTREEITEMMODEL_H

#include "cxBaseWidget.h"

#include <QtGui>
#include "cxImage.h"
#include "cxTreeNode.h"
#include "cxStringProperty.h"
#include "cxXmlOptionItem.h"
#include "cxLabeledComboBoxWidget.h"

class QTreeWidget;
class QVBoxLayout;
class QTreeWidgetItem;
class QTreeView;

namespace cx
{

typedef boost::shared_ptr<class StringListProperty> StringListPropertyPtr;
typedef boost::shared_ptr<class TreeRepository> TreeRepositoryPtr;

/**
 * \ingroup cxGUI
 *
 */
class TreeItemModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  TreeItemModel(XmlOptionFile options, VisServicesPtr services, QObject* parent = 0);
  virtual ~TreeItemModel();
  void setSelectionModel(QItemSelectionModel* selectionModel);

  virtual int columnCount (const QModelIndex& parent = QModelIndex() ) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
  virtual Qt::ItemFlags flags(const QModelIndex& index ) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex& index ) const;

  TreeNodePtr getCurrentItem();
  TreeNodePtr getNodeFromIndex(const QModelIndex& index);
  TreeRepositoryPtr repo() { return mRepository; }
  StringListPropertyPtr getShowColumnsProperty() { return mShowColumnsProperty; }

signals:
//  void hasBeenReset();
  void loaded();
  void currentItemChanged();

public slots:
  void update();

private slots:
  void currentItemChangedSlot(const QModelIndex& current, const QModelIndex& previous);
  void onRepositoryChanged(TreeNode* node);

private:
  int mViewGroupCount, mViewGroupIndex, mColorIndex, mNameIndex, mColumnCount;

  TreeRepositoryPtr mRepository;
  QItemSelectionModel* mSelectionModel;
  XmlOptionFile mOptions;
  VisServicesPtr mServices;
  StringListPropertyPtr mShowColumnsProperty;

  bool isViewGroupColumn(int col) const;
  int viewGroupFromColumn(int col) const;

  TreeNode* itemFromIndex(const QModelIndex& index) const;
  void fillModelTreeFromViewManager(TreeNodePtr root);
  QIcon getColorIcon(QColor color) const;
  QColor adjustColorToContrastWithWhite(QColor color) const;
  void createShowColumnsProperty();
  void onShowColumnsChanged();
};

}//end namespace cx


#endif // CXTREEITEMMODEL_H
