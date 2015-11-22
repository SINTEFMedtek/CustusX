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
#include "cxDataTreeNode.h"
#include "cxPatientModelService.h"
#include "cxDefinitions.h"
#include "cxData.h"
#include "cxTreeRepository.h"
#include "cxLogger.h"
#include "cxDataMetric.h"
#include "cxActiveData.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxVisServices.h"
#include <QFont>

namespace cx
{


DataTreeNode::DataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data) :
	TreeNodeImpl(repo), mData(data)
{
}

QString DataTreeNode::getUid() const
{
	return mData->getUid();
}

QString DataTreeNode::getName() const
{
	return mData->getName();
}

QString DataTreeNode::getType() const
{
	return "data";
}

TreeNodePtr DataTreeNode::getParent() const
{
	if (mData->getParentSpace().isEmpty())
		return this->repo()->getNode(CoordinateSystem(csREF).toString());
	TreeNodePtr node = this->repo()->getNode(mData->getParentSpace());
	if (node)
		return node;
	node = this->repo()->getNode(CoordinateSystem(csDATA, mData->getParentSpace()).toString());
	return node;
}

void DataTreeNode::activate()
{
	this->getServices()->patient()->getActiveData()->setActive(mData);
}

QIcon DataTreeNode::getIcon() const
{
	return mData->getIcon();
}

QVariant DataTreeNode::getColor() const
{
	DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(mData);
	if (metric)
		return metric->getColor();
	return QColor("black");
}

QVariant DataTreeNode::getFont() const
{
	if (this->getServices()->patient()->getActiveData()->getActive()==mData)
	{
		QFont font;
		font.setBold(true);
		return font;
	}
	return QVariant();
}

QVariant DataTreeNode::getViewGroupVisibility(int index) const
{
	DataViewProperties props = this->getServices()->view()->getGroup(index)->getProperties(mData->getUid());
	if (props.empty())
		return Qt::CheckState(0);
	if ((props.hasVolume3D() || props.hasSlice3D()) && props.hasSlice2D())
		return Qt::CheckState(2);
	return Qt::CheckState(1);
//	return true;
}

void DataTreeNode::setViewGroupVisibility(int index, bool value)
{
	if (value)
		this->getServices()->view()->getGroup(index)->setProperties(mData->getUid(), DataViewProperties::createDefault());
	else
		this->getServices()->view()->getGroup(index)->setProperties(mData->getUid(), DataViewProperties());
}


} // namespace cx
