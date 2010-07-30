/*
 * cxTreeModelItem.cpp
 *
 *  Created on: Jul 30, 2010
 *      Author: christiana
 */

#include "cxTreeModelItem.h"
#include "cxDataManager.h"
#include "sscTypeConversions.h"
#include "cxToolManager.h"

namespace cx
{

TreeItemPtr TreeItemImpl::create(TreeItemWeakPtr parent, QString name, QString type, QString val)
{
  TreeItemPtr retval(new TreeItemImpl(parent,name,type,val));
  if (parent.lock())
    parent.lock()->addChild(retval);
  return retval;
}

TreeItemImpl::TreeItemImpl(TreeItemWeakPtr parent, QString name, QString type, QString val) :
  mParent(parent),
  mName(name), mType(type), mData(val)
{

}
TreeItemPtr TreeItemImpl::addChild(TreeItemPtr child)
{
  mChildren.push_back(child);
  return child;
}
TreeItemPtr TreeItemImpl::getChild(int row)
{
  return mChildren[row];
}

QString TreeItemImpl::getName() const { return mName; }
QString TreeItemImpl::getType() const { return mType; }
QString TreeItemImpl::getData() const  { return mData; }

int TreeItemImpl::getChildCount() const { return mChildren.size(); }
int TreeItemImpl::getColumnCount() const { return 3; }
TreeItemWeakPtr TreeItemImpl::getParent()
{
  return mParent;
}
void TreeItemImpl::activate()
{
  // ignore
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


TreeItemPtr TreeItemImage::create(TreeItemWeakPtr parent, std::string uid)
{
  TreeItemPtr retval(new TreeItemImage(parent,uid));
  if (parent.lock())
    parent.lock()->addChild(retval);
  return retval;
}

QString TreeItemImage::getName() const
{
  if (!dataManager()->getImage(mUid))
    return "";
  return qstring_cast(dataManager()->getImage(mUid)->getName());
}

void TreeItemImage::activate()
{
  dataManager()->setActiveImage(dataManager()->getImage(mUid));
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

TreeItemPtr TreeItemToolManager::create(TreeItemWeakPtr parent)
{
  TreeItemPtr retval(new TreeItemToolManager(parent));
  if (parent.lock())
    parent.lock()->addChild(retval);

  ssc::ToolManager::ToolMapPtr tools = toolManager()->getTools();
  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    TreeItemTool::create(retval, iter->second);
  }

  return retval;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

TreeItemPtr TreeItemTool::create(TreeItemWeakPtr parent, ssc::ToolPtr tool)
{
  TreeItemPtr retval(new TreeItemTool(parent, tool));
  if (parent.lock())
    parent.lock()->addChild(retval);
  return retval;
}

QString TreeItemTool::getName() const
{
  return qstring_cast(mTool->getName());
}


} // namespace cx
