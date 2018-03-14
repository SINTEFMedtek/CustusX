/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXDATATREENODE_H
#define CXDATATREENODE_H

#include "cxTreeNodeImpl.h"

namespace cx
{


class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;


class DataTreeNode : public TreeNodeImpl
{
  Q_OBJECT
public:
	DataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data);
	virtual ~DataTreeNode();
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual QString getType() const;
	virtual TreeNodePtr getParent() const;
	virtual bool isVisibleNode() const;
	virtual void activate();
	virtual QIcon getIcon() const;
	virtual QVariant getViewGroupVisibility(int index) const;
	virtual void setViewGroupVisibility(int index, bool value);
	virtual boost::shared_ptr<QWidget> createPropertiesWidget() const;
	virtual QVariant getColor() const;
	virtual QVariant getFont() const;
	virtual bool isDefaultExpanded() const;

	virtual bool isRemovable() const { return true; }
	virtual void remove();

private:
	DataPtr mData;
};

} // namespace cx

#endif // CXDATATREENODE_H
