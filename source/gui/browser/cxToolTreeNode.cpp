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
#include "cxToolTreeNode.h"
#include "cxPatientModelService.h"
#include "cxDefinitions.h"
#include "cxData.h"
#include "cxLegacySingletons.h"
#include "cxTreeRepository.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include <QFont>

namespace cx
{


ToolTreeNode::ToolTreeNode(TreeRepositoryWeakPtr repo, ToolPtr tool) :
	TreeNodeImpl(repo), mTool(tool)
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
	if (mTool->hasType(Tool::TOOL_REFERENCE))
		return this->repo()->getNode(CoordinateSystem(csPATIENTREF).toString());
//		return this->repo()->getNode(CoordinateSystem(csREF).toString());
	ToolPtr ref = trackingService()->getReferenceTool();
	if (ref)
		return this->repo()->getNode(ref->getUid());
	return TreeNodePtr();
}

void ToolTreeNode::activate()
{
	trackingService()->setActiveTool(mTool->getUid());
}

QIcon ToolTreeNode::getIcon() const
{
	return QIcon(":icons/polaris-green.png");
//	return mTool->getIcon();
}

QVariant ToolTreeNode::getColor() const
{
	if (!mTool->isInitialized())
		return QColor("darkgray");
	if (mTool->getVisible())
		return QColor("green");
	else
		return QColor("red");

	CX_LOG_CHANNEL_DEBUG("CA") << "tool " << mTool->getName() << ": init=" << mTool->isInitialized() << ", v=" << mTool->getVisible();
}

QVariant ToolTreeNode::getFont() const
{
	if (trackingService()->getActiveTool()==mTool)
	{
		QFont font;
		font.setBold(true);
		return font;
	}
	return QVariant();
}

} // namespace cx
