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
#ifndef CXTREENODEIMPL_H
#define CXTREENODEIMPL_H

#include "cxTreeNode.h"
#include "cxCoordinateSystemHelpers.h"
#include <QVariant>

namespace cx
{

class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;


class TreeNodeImpl : public TreeNode
{
  Q_OBJECT
public:
	TreeNodeImpl(TreeRepositoryWeakPtr repo);
	virtual ~TreeNodeImpl() {}
	virtual std::vector<TreeNodePtr> getChildren() const;
	virtual bool isVisibleNode() const;

	virtual void activate() {}
	virtual QVariant getViewGroupVisibility(int index) const { return QVariant(); }
	virtual void setViewGroupVisibility(int index, bool value) {}
	virtual QVariant getColor() const { return QVariant(); }
	virtual bool  useColoredName() const { return false; }
	virtual QVariant getFont() const { return QVariant(); }
	virtual QWidget* createPropertiesWidget() const { return NULL; }

	virtual std::vector<TreeNodePtr> getVisibleChildren() const;
	virtual TreeNodePtr getVisibleParent() const;

protected:
	TreeRepositoryWeakPtr mRepository;
	VisServicesPtr getServices() const;

	TreeRepositoryPtr repo();
	const TreeRepositoryPtr repo() const;

	QIcon addBackgroundColorToIcon(QIcon input, QColor color) const;
};

} // namespace cx

#endif // CXTREENODEIMPL_H
