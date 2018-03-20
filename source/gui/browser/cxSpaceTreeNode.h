/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSPACETREENODE_H
#define CXSPACETREENODE_H

#include "cxTreeNodeImpl.h"

namespace cx
{

class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;


class SpaceTreeNode : public TreeNodeImpl
{
  Q_OBJECT
public:
	SpaceTreeNode(TreeRepositoryWeakPtr repo, CoordinateSystem space);
	virtual ~SpaceTreeNode() {}
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual QString getType() const;
	virtual TreeNodePtr getParent() const;
	virtual bool isVisibleNode() const;
	virtual QIcon getIcon() const;

private:
	CoordinateSystem mSpace;
};

} // namespace cx

#endif // CXSPACETREENODE_H
