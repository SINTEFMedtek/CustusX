/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
