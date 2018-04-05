/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVIEWSERVICETREENODE_H
#define CXVIEWSERVICETREENODE_H

#include "cxTreeNodeImpl.h"
#include "cxGroupTreeNode.h"

namespace cx
{

class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;


class ViewServiceTreeNode : public GroupTreeNode
{
  Q_OBJECT
public:
	ViewServiceTreeNode(TreeRepositoryWeakPtr repo);
	virtual ~ViewServiceTreeNode() {}
//	virtual QString getUid() const;
//	virtual QString getName() const;
//	virtual QString getType() const;
//	virtual TreeNodePtr getParent() const;
//	virtual void activate();
//	virtual QIcon getIcon() const;
//	QWidget* createPropertiesWidget() const;
//	virtual QVariant getColor() const;
//	virtual bool useColoredName() const { return true; }
//	virtual QVariant getFont() const;

private:
};

} // namespace cx

#endif // CXVIEWSERVICETREENODE_H
