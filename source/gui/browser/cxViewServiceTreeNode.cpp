/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxViewServiceTreeNode.h"

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


ViewServiceTreeNode::ViewServiceTreeNode(TreeRepositoryWeakPtr repo) :
	GroupTreeNode(repo, "view")
{

}

//QString ViewServiceTreeNode::getUid() const
//{
//	return mTool->getUid();
//}

//QString ViewServiceTreeNode::getName() const
//{
//	return mTool->getName();
//}

//TreeNodePtr ViewServiceTreeNode::getParent() const
//{
//	if (this->repo()->getMode()=="flat")
//		return this->repo()->getNodeForGroup("tool");

//	if (mTool->hasType(Tool::TOOL_REFERENCE))
//		return this->repo()->getNode(CoordinateSystem(csPATIENTREF).toString());
////		return this->repo()->getNode(CoordinateSystem(csREF).toString());
//	ToolPtr ref = this->getServices()->tracking()->getReferenceTool();
//	if (ref)
//		return this->repo()->getNode(ref->getUid());
//	return TreeNodePtr();
//}

//void ViewServiceTreeNode::activate()
//{
//	this->getServices()->tracking()->setActiveTool(mTool->getUid());
//}

//QString ViewServiceTreeNode::getType() const
//{
//	return "tool";
//}

//QIcon ViewServiceTreeNode::getIcon() const
//{
//	if (mTool->hasType(Tool::TOOL_US_PROBE))
//		return QIcon(":icons/tool_us_probe.png");
//	if (mTool->hasType(Tool::TOOL_POINTER))
//		return QIcon(":icons/tool_pointer.png");
//	if (mTool->hasType(Tool::TOOL_REFERENCE))
//		return QIcon(":icons/tool_reference.png");
//	if (mTool->hasType(Tool::TOOL_MANUAL))
//		return QIcon(":icons/tool_manual.png");
//	return QIcon(":icons/tool_pointer.png");
//}

//QVariant ViewServiceTreeNode::getColor() const
//{
//	if (!mTool->isInitialized())
//		return QColor("darkgray");
//	if (mTool->getVisible())
//		return QColor("green");
//	else
//		return QColor("red");

//	CX_LOG_CHANNEL_DEBUG("CA") << "tool " << mTool->getName() << ": init=" << mTool->isInitialized() << ", v=" << mTool->getVisible();
//}

//QVariant ViewServiceTreeNode::getFont() const
//{
//	if (this->getServices()->tracking()->getActiveTool()==mTool)
//	{
//		QFont font;
//		font.setBold(true);
//		return font;
//	}
//	return QVariant();
//}

//QWidget* ViewServiceTreeNode::createPropertiesWidget() const
//{
//	StringPropertySelectToolPtr selector = StringPropertySelectTool::New(this->getServices()->tracking());
//	selector->setValue(mTool->getUid());
//	return new ToolPropertiesWidget(selector,
//							  this->getServices()->tracking(),
//							  this->getServices()->spaceProvider(),
//							  NULL);

////	return new ToolPropertiesWidget(NULL);
////	return new QLabel(QString("Tool widget %1 ").arg(mTool->getName()));
//}


} // namespace cx
