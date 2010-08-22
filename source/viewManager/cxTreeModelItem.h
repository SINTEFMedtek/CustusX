/*
 * cxTreeModelItem.h
 *
 *  Created on: Jul 30, 2010
 *      Author: christiana
 */

#ifndef CXTREEMODELITEM_H_
#define CXTREEMODELITEM_H_

#include <vector>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include <QFont>
#include "sscForwardDeclarations.h"

namespace cx
{

class BrowserItemModel;
class TreeItem;
typedef boost::weak_ptr<TreeItem> TreeItemWeakPtr;
typedef boost::shared_ptr<TreeItem> TreeItemPtr;


class TreeItem : public QObject
{
  Q_OBJECT
public:
  virtual ~TreeItem() {}
  virtual TreeItemPtr getChild(int row) = 0;
  virtual TreeItemPtr addChild(TreeItemPtr child) = 0;
  virtual QString getName() const = 0;
  virtual QString getType() const = 0;
  virtual QString getData() const = 0;
  virtual QFont getFont() const = 0;
  virtual int getChildCount() const = 0;
  virtual int getColumnCount() const = 0;
  virtual TreeItemWeakPtr getParent() = 0;
  virtual void activate() = 0;
  virtual BrowserItemModel* getModel() = 0;

signals:
  void changed();
};

class TreeItemImpl : public TreeItem
{
public:
  virtual ~TreeItemImpl() {}
  static TreeItemPtr create(BrowserItemModel* model);
  static TreeItemPtr create(TreeItemWeakPtr parent, QString name, QString type, QString val);
  virtual TreeItemPtr addChild(TreeItemPtr child);
  virtual TreeItemPtr getChild(int row);
  virtual QString getName() const;
  virtual QString getType() const;
  virtual QString getData() const;
  virtual QFont getFont() const { return QFont(); }

  virtual int getChildCount() const;
  virtual int getColumnCount() const;
  //QVariant data(int column) const;
  virtual TreeItemWeakPtr getParent();
  virtual void activate();
  virtual BrowserItemModel* getModel() { return mModel; }

protected:
  TreeItemImpl(TreeItemWeakPtr parent, QString name, QString type, QString val);
  TreeItemWeakPtr mParent;
  std::vector<TreeItemPtr> mChildren;

  QString mName;
  QString mType;
  QString mData;
  BrowserItemModel* mModel;
};

class TreeItemImage : public TreeItemImpl
{
public:
  static TreeItemPtr create(TreeItemWeakPtr parent, std::string uid);
  virtual ~TreeItemImage();
  virtual QString getName() const;
  virtual void activate();
  virtual QFont getFont() const;

private:
  TreeItemImage(TreeItemWeakPtr parent, std::string uid);
  std::string mUid;
};

class TreeItemToolManager : public TreeItemImpl
{
public:
  static TreeItemPtr create(TreeItemWeakPtr parent);
  virtual ~TreeItemToolManager() {}
private:
  TreeItemToolManager(TreeItemWeakPtr parent) : TreeItemImpl(parent,"tracking","manager","Tool Manager")  {}
};

class TreeItemTool : public TreeItemImpl
{
public:
  static TreeItemPtr create(TreeItemWeakPtr parent, ssc::ToolPtr tool);
  virtual ~TreeItemTool() {}
  virtual QString getName() const;
private:
  TreeItemTool(TreeItemWeakPtr parent, ssc::ToolPtr tool) : TreeItemImpl(parent,"","tool",""), mTool(tool) {}
  ssc::ToolPtr mTool;
};

}

#endif /* CXTREEMODELITEM_H_ */
