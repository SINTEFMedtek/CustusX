/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTreeNode.h"


namespace cx
{

CachedTreeNode::CachedTreeNode(TreeNodePtr base) : mBase(base)
{
	connect(mBase.get(), &TreeNode::changed, this, &CachedTreeNode::clearCache);
	connect(mBase.get(), &TreeNode::changed, this, &TreeNode::changed);
	mVisibleChildrenSet = false;
}

void CachedTreeNode::clearCache()
{
	mUid = QString();
	mType = QString();
	mVisibleParent.reset();
	mVisibleChildren.clear();
	mVisibleChildrenSet = false;
}

QString CachedTreeNode::getUid() const
{
	if (mUid.isEmpty())
		mUid = mBase->getUid();
	return mUid;
}

QString CachedTreeNode::getType() const
{
	if (mType.isEmpty())
		mType = mBase->getType();
	return mType;
}

std::vector<TreeNodePtr> CachedTreeNode::getVisibleChildren() const
{
//	return mBase->getVisibleChildren();
	if (!mVisibleChildrenSet)
	{
		std::vector<TreeNodePtr> children = mBase->getVisibleChildren();

		mVisibleChildren = std::vector<TreeNodeWeakPtr>(children.begin(), children.end());
		mVisibleChildrenSet = true;
	}

	std::vector<TreeNodePtr> retval(mVisibleChildren.size());
	for (unsigned i=0; i< retval.size(); ++i)
		retval[i] = mVisibleChildren[i].lock();
	return retval;
}

TreeNodePtr CachedTreeNode::getVisibleParent() const
{
//	return mBase->getVisibleParent();
	if (!mVisibleParent.lock())
		mVisibleParent = mBase->getVisibleParent();
	return mVisibleParent.lock();
}


} // namespace cx
