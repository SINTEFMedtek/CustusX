/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	virtual boost::shared_ptr<QWidget> createPropertiesWidget() const { return boost::shared_ptr<QWidget>(); }
	virtual bool isDefaultExpanded() const { return true; }

	virtual bool isRemovable() const { return false; }
	virtual void remove() { }

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
