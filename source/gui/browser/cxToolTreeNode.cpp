/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxToolTreeNode.h"
#include "cxTreeRepository.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxVisServices.h"
#include <QIcon>
#include <QFont>
#include <QLabel>
#include "cxToolPropertiesWidget.h"
#include "cxStringPropertySelectTool.h"

namespace cx
{


ToolTreeNode::ToolTreeNode(TreeRepositoryWeakPtr repo, ToolPtr tool) :
	TreeNodeImpl(repo), mTool(tool)
{
	connect(mTool.get(), &Tool::toolVisible, this, &TreeNode::changed);

}

ToolTreeNode::~ToolTreeNode()
{
}

QString ToolTreeNode::getUid() const
{
	return mTool->getUid();
}

QString ToolTreeNode::getName() const
{
	return mTool->getName();
}

TreeNodePtr ToolTreeNode::getParent() const
{
	if (this->repo()->getMode()=="flat")
		return this->repo()->getNodeForGroup("tool");

	if (mTool->getUid() == this->getServices()->tracking()->getManualTool()->getUid())
		return this->repo()->getNode(CoordinateSystem(csPATIENTREF).toString());

	if (mTool->hasType(Tool::TOOL_REFERENCE))
		return this->repo()->getNode(CoordinateSystem(csPATIENTREF).toString());

	ToolPtr ref = this->getServices()->tracking()->getReferenceTool();
	if (ref)
		return this->repo()->getNode(ref->getUid());

	return this->repo()->getNode(CoordinateSystem(csPATIENTREF).toString());
}

void ToolTreeNode::activate()
{
	this->getServices()->tracking()->setActiveTool(mTool->getUid());
}

QString ToolTreeNode::getType() const
{
	return "tool";
}

QIcon ToolTreeNode::getIcon() const
{
	if (mTool->hasType(Tool::TOOL_US_PROBE))
		return QIcon(":icons/tool_us_probe.png");
	if (mTool->hasType(Tool::TOOL_POINTER))
		return QIcon(":icons/tool_pointer.png");
	if (mTool->hasType(Tool::TOOL_REFERENCE))
		return QIcon(":icons/tool_reference.png");
	if (mTool->hasType(Tool::TOOL_MANUAL))
		return QIcon(":icons/tool_manual.png");
	return QIcon(":icons/tool_pointer.png");
}

QVariant ToolTreeNode::getColor() const
{
	if (!mTool->isInitialized())
		return QColor("darkgray");
	if (mTool->getVisible())
		return QColor("green");
	else
		return QColor("red");
}

QVariant ToolTreeNode::getFont() const
{
	if (this->getServices()->tracking()->getActiveTool()==mTool)
	{
		QFont font;
		font.setBold(true);
		return font;
	}
	return QVariant();
}

boost::shared_ptr<QWidget> ToolTreeNode::createPropertiesWidget() const
{
	StringPropertySelectToolPtr selector = StringPropertySelectTool::New(this->getServices()->tracking());
	selector->setValue(mTool->getUid());
	return boost::shared_ptr<QWidget>(new ToolPropertiesWidget(selector,
															   this->getServices()->tracking(),
															   this->getServices()->spaceProvider(),
															   NULL));

//	return new ToolPropertiesWidget(NULL);
//	return new QLabel(QString("Tool widget %1 ").arg(mTool->getName()));
}


} // namespace cx
