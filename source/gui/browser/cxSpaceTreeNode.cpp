/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxSpaceTreeNode.h"
#include "cxLogger.h"
#include "cxTreeRepository.h"
#include <QIcon>

namespace cx
{


SpaceTreeNode::SpaceTreeNode(TreeRepositoryWeakPtr repo, CoordinateSystem space) :
	TreeNodeImpl(repo),
	mSpace(space)
{

}

QString SpaceTreeNode::getUid() const
{
	return mSpace.toString();
}

QString SpaceTreeNode::getName() const
{
	return mSpace.toString();
}

QString SpaceTreeNode::getType() const
{
	return "space";
}

bool SpaceTreeNode::isVisibleNode() const
{
	if (mSpace.mId == csPATIENTREF)
	{
		bool toolsVisible = this->repo()->getVisibleNodeTypes().contains("tool");
		return toolsVisible;
	}

	return true;
}

TreeNodePtr SpaceTreeNode::getParent() const
{
	if (this->repo()->getMode()=="flat")
		return this->repo()->getNodeForGroup("space");

	if (mSpace.mId == csREF)
	{
		return this->repo()->getTopNode();
	}
	if (mSpace.mId == csPATIENTREF)
	{
		return this->repo()->getNode(CoordinateSystem(csREF).toString());
	}
	else if (mSpace.mId == csDATA)
	{
		return this->repo()->getNode(CoordinateSystem(csREF).toString());
	}
	else
	{
		CX_LOG_CHANNEL_DEBUG("CA") << "TBD: Not implemented";
		return TreeNodePtr();
	}
}

QIcon SpaceTreeNode::getIcon() const
{
	if (mSpace.mId == csREF)
	{
		return QIcon(":/icons/space_reference.png");
	}
	if (mSpace.mId == csPATIENTREF)
	{
		return QIcon(":/icons/space_patient_reference.png");
	}
	else
	{
		return QIcon(":/icons/space_generic.png");
	}
}



} // namespace cx
