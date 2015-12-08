/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxTreeNode.h"


namespace cx
{

CachedTreeNode::CachedTreeNode(TreeNodePtr base) : mBase(base)
{
	connect(mBase.get(), &TreeNode::changed, this, &CachedTreeNode::clearCache);
	connect(mBase.get(), &TreeNode::changed, this, &TreeNode::changed);
}

void CachedTreeNode::clearCache()
{
	mUid = QString();
	mType = QString();
	mVisibleParent.reset();
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
	return mBase->getVisibleChildren();
}

TreeNodePtr CachedTreeNode::getVisibleParent() const
{
	return mBase->getVisibleParent();
//	if (!mVisibleParent.lock())
//		mVisibleParent = mBase->getVisibleParent();
//	return mVisibleParent.lock();
}


} // namespace cx
