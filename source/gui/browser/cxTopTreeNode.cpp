/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTopTreeNode.h"

#include <QIcon>

namespace cx
{


TopTreeNode::TopTreeNode(TreeRepositoryWeakPtr repo) :
	TreeNodeImpl(repo)
{

}

QString TopTreeNode::getUid() const
{
	return "node::invisible_top";
}

QString TopTreeNode::getName() const
{
	return "";
}

QString TopTreeNode::getType() const
{
	return "top";
}

TreeNodePtr TopTreeNode::getParent() const
{
	return TreeNodePtr();
}

bool TopTreeNode::isVisibleNode() const
{
	return true;
}

QIcon TopTreeNode::getIcon() const
{
	return QIcon();
}


} // namespace cx

