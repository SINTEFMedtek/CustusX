/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXGROUPTREENODE_H
#define CXGROUPTREENODE_H

#include "cxTreeNodeImpl.h"

namespace cx
{

class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;


class GroupTreeNode : public TreeNodeImpl
{
  Q_OBJECT
public:
	GroupTreeNode(TreeRepositoryWeakPtr repo, QString typeName);
	virtual ~GroupTreeNode() {}
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual QString getType() const;
	virtual TreeNodePtr getParent() const;
	virtual bool isVisibleNode() const;
	virtual QIcon getIcon() const;

private:
	QString mTypeName;
};

} // namespace cx

#endif // CXGROUPTREENODE_H
