/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxViewGroupTreeNode.h"
#include "cxTreeRepository.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxVisServices.h"
#include <QIcon>
#include <QFont>
#include <QLabel>
#include "cxDoubleProperty.h"
#include "cxViewGroupPropertiesWidget.h"
#include "cxViewService.h"

namespace cx
{


ViewGroupTreeNode::ViewGroupTreeNode(TreeRepositoryWeakPtr repo, int groupIndex) :
	TreeNodeImpl(repo), mGroupIndex(groupIndex)
{

}

QString ViewGroupTreeNode::getUid() const
{
	return QString("view_%1").arg(mGroupIndex);
}

QString ViewGroupTreeNode::getName() const
{
	return QString("View %1").arg(mGroupIndex);
}

TreeNodePtr ViewGroupTreeNode::getParent() const
{
	if (this->repo()->getMode()=="flat")
		return this->repo()->getNodeForGroup("view");
	return TreeNodePtr();
}

void ViewGroupTreeNode::activate()
{
//	this->getServices()->view()->set... need a method for setting active view group
}

QString ViewGroupTreeNode::getType() const
{
	return "view";
}

QIcon ViewGroupTreeNode::getIcon() const
{
	return QIcon(":icons/open_icon_library/eye.png.png");
}

QVariant ViewGroupTreeNode::getColor() const
{
	return QColor("blue");
}

QVariant ViewGroupTreeNode::getFont() const
{
	return QVariant();
}

boost::shared_ptr<QWidget> ViewGroupTreeNode::createPropertiesWidget() const
{
	return boost::shared_ptr<QWidget>(new ViewGroupPropertiesWidget(mGroupIndex,
																	this->getServices(),
																	NULL));
}


} // namespace cx

