/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTOPTREENODE_H
#define CXTOPTREENODE_H

#include "cxTreeNodeImpl.h"

namespace cx
{

class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;

class TopTreeNode : public TreeNodeImpl
{
  Q_OBJECT
public:
	TopTreeNode(TreeRepositoryWeakPtr repo);
	virtual ~TopTreeNode() {}
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual QString getType() const;
	virtual TreeNodePtr getParent() const;
	virtual bool isVisibleNode() const;
	virtual QIcon getIcon() const;

private:
};

} // namespace cx

#endif // CXTOPTREENODE_H
