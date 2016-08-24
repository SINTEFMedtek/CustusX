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
#ifndef CXSHOWDATATREENODE_H
#define CXSHOWDATATREENODE_H

#include "cxTreeNodeImpl.h"
#include "cxViewGroupData.h"

namespace cx
{


class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;

class ShowDataTreeNodeBase : public TreeNodeImpl
{
  Q_OBJECT
public:
	ShowDataTreeNodeBase(TreeRepositoryWeakPtr repo, DataPtr data);
	virtual ~ShowDataTreeNodeBase() {}
//	virtual QString getUid() const;
//	virtual QString getName() const;
//	virtual QString getType() const;
	virtual TreeNodePtr getParent() const;
	virtual bool isVisibleNode() const;
	virtual void activate();
	virtual QIcon getIcon() const;
	virtual QVariant getViewGroupVisibility(int index) const;
	virtual void setViewGroupVisibility(int index, bool value);
	virtual boost::shared_ptr<QWidget> createPropertiesWidget() const;
	virtual QVariant getColor() const;
	virtual QVariant getFont() const;

protected:
	virtual DataViewProperties getDefiningDataViewProperties() const = 0;
	DataPtr mData;
};

class ShowVolumeDataTreeNode : public ShowDataTreeNodeBase
{
  Q_OBJECT
public:
	ShowVolumeDataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data);
	virtual ~ShowVolumeDataTreeNode() {}
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual QString getType() const;

protected:
	virtual DataViewProperties getDefiningDataViewProperties() const;
};

class ShowSlice2DDataTreeNode : public ShowDataTreeNodeBase
{
  Q_OBJECT
public:
	ShowSlice2DDataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data);
	virtual ~ShowSlice2DDataTreeNode() {}
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual QString getType() const;

protected:
	virtual DataViewProperties getDefiningDataViewProperties() const;
};

class ShowSlice3DDataTreeNode : public ShowDataTreeNodeBase
{
  Q_OBJECT
public:
	ShowSlice3DDataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data);
	virtual ~ShowSlice3DDataTreeNode() {}
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual QString getType() const;

protected:
	virtual DataViewProperties getDefiningDataViewProperties() const;
};

} // namespace cx

#endif // CXSHOWDATATREENODE_H
