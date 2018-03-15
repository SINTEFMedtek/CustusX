/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxGroupTreeNode.h"

#include "cxLogger.h"
#include "cxTreeRepository.h"
#include <QIcon>

namespace cx
{


GroupTreeNode::GroupTreeNode(TreeRepositoryWeakPtr repo, QString typeName) :
	TreeNodeImpl(repo),
	mTypeName(typeName)
{

}

QString GroupTreeNode::getUid() const
{
	return QString("group_%1").arg(mTypeName);
}

QString GroupTreeNode::getName() const
{
	return QString("%1 group").arg(mTypeName);
}

QString GroupTreeNode::getType() const
{
	return "group";
}

bool GroupTreeNode::isVisibleNode() const
{
	if (this->repo()->getMode()!="flat")
		return false;
	return TreeNodeImpl::isVisibleNode();
}

TreeNodePtr GroupTreeNode::getParent() const
{
	if (this->repo()->getMode()!="flat")
		return TreeNodePtr();
	return this->repo()->getTopNode();
}

QIcon GroupTreeNode::getIcon() const
{
	return QIcon(":icons/open_icon_library/document-open-7.png");
}



} // namespace cx

