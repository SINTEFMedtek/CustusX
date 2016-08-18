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

