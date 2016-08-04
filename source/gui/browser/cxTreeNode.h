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

#ifndef CXTREENODE_H
#define CXTREENODE_H

#include <vector>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "cxForwardDeclarations.h"
#include <QIcon>
#include <QVariant>

namespace cx
{

class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;
typedef boost::weak_ptr<class TreeRepository> TreeRepositoryWeakPtr;
typedef boost::shared_ptr<class TreeRepository> TreeRepositoryPtr;


class TreeNode : public QObject
{
  Q_OBJECT
public:
	virtual ~TreeNode() {}
	virtual std::vector<TreeNodePtr> getChildren() const = 0;
	virtual QString getUid() const = 0;
	virtual QString getName() const = 0;
	virtual QString getType() const = 0;
	virtual TreeNodePtr getParent() const = 0;
	virtual bool isVisibleNode() const = 0;
	virtual void activate() = 0;
	virtual QIcon getIcon() const = 0;
	virtual QVariant getViewGroupVisibility(int index) const = 0;
	virtual void setViewGroupVisibility(int index, bool value) = 0;
	virtual QWidget* createPropertiesWidget() const = 0;
	virtual QVariant getColor() const = 0;
	virtual bool  useColoredName() const = 0;
	virtual QVariant getFont() const = 0;
	virtual bool isDefaultExpanded() const = 0;

	virtual bool isRemovable() const = 0;
	virtual void remove() = 0;

	virtual std::vector<TreeNodePtr> getVisibleChildren() const = 0;
	virtual TreeNodePtr getVisibleParent() const = 0;

signals:
	void parentChanged(TreeNodePtr prev, TreeNodePtr next);
	void changed();
};

/**
 * Cache for some often-used parts of TreeNode,
 * that is unchanged between each reset.
 */
class CachedTreeNode : public TreeNode
{
  Q_OBJECT
public:
	explicit CachedTreeNode(TreeNodePtr base);
	virtual ~CachedTreeNode() {}
	virtual std::vector<TreeNodePtr> getChildren() const { return mBase->getChildren(); }
	virtual QString getUid() const;
	virtual QString getName() const { return mBase->getName(); }
	virtual QString getType() const;
	virtual TreeNodePtr getParent() const { return mBase->getParent(); }
	virtual bool isVisibleNode() const { return mBase->isVisibleNode(); }
	virtual void activate() { mBase->activate(); }
	virtual QIcon getIcon() const { return mBase->getIcon(); }
	virtual QVariant getViewGroupVisibility(int index) const { return mBase->getViewGroupVisibility(index); }
	virtual void setViewGroupVisibility(int index, bool value) { mBase->setViewGroupVisibility(index, value); }
	virtual QWidget* createPropertiesWidget() const { return mBase->createPropertiesWidget(); }
	virtual QVariant getColor() const { return mBase->getColor(); }
	virtual bool  useColoredName() const { return mBase->useColoredName(); }
	virtual QVariant getFont() const { return mBase->getFont(); }
	virtual bool isDefaultExpanded() const { return mBase->isDefaultExpanded(); }

	virtual bool isRemovable() const { return mBase->isRemovable(); }
	virtual void remove() { mBase->remove(); }

	virtual std::vector<TreeNodePtr> getVisibleChildren() const;
	virtual TreeNodePtr getVisibleParent() const;

private:
	mutable QString mUid;
	mutable QString mType;
	mutable std::vector<TreeNodeWeakPtr> mVisibleChildren;
	mutable bool mVisibleChildrenSet;
	mutable TreeNodeWeakPtr mVisibleParent;
	void clearCache();

	TreeNodePtr mBase;
};

} // namespace cx

#endif // CXTREENODE_H
