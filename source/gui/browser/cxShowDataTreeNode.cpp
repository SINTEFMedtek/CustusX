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
#include "cxShowDataTreeNode.h"
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
#include <QLabel>
#include "cxMesh.h"
#include "cxMeshInfoWidget.h"
#include "cxImagePropertiesWidget.h"
#include "cxImage.h"
#include "cxSelectDataStringProperty.h"
#include "cxDataMetric.h"

namespace cx
{


ShowDataTreeNodeBase::ShowDataTreeNodeBase(TreeRepositoryWeakPtr repo, DataPtr data) :
	TreeNodeImpl(repo), mData(data)
{
}

bool ShowDataTreeNodeBase::isVisibleNode() const
{
	bool flatMode = this->repo()->getMode()=="flat";
	bool parentVisible = this->getParent()->isVisibleNode();
	return parentVisible && flatMode;
}

TreeNodePtr ShowDataTreeNodeBase::getParent() const
{
	return this->repo()->getNode(mData->getUid());
}

void ShowDataTreeNodeBase::activate()
{
	this->getParent()->activate();
}

QIcon ShowDataTreeNodeBase::getIcon() const
{
	return QIcon();
}

QVariant ShowDataTreeNodeBase::getColor() const
{
	return this->getParent()->getColor();
//	return QColor("black");
}

QVariant ShowDataTreeNodeBase::getFont() const
{
	return QVariant();
}

QVariant ShowDataTreeNodeBase::getViewGroupVisibility(int index) const
{
	ViewGroupDataPtr group = this->getServices()->view()->getGroup(index);
	DataViewProperties props = group->getProperties(mData->getUid());

	//	if ((props.hasVolume3D() || props.hasSlice3D()) && props.hasSlice2D())
	if (props.containsAnyFlagsIn(this->getDefiningDataViewProperties()))
		return Qt::CheckState(2);
	else
		return Qt::CheckState(0);
}

void ShowDataTreeNodeBase::setViewGroupVisibility(int index, bool value)
{
	ViewGroupDataPtr group = this->getServices()->view()->getGroup(index);
	DataViewProperties props = group->getProperties(mData->getUid());

	if (value)
		props.addFlagsIn(this->getDefiningDataViewProperties());
	else
		props.removeFlagsIn(this->getDefiningDataViewProperties());

	group->setProperties(mData->getUid(), props);
}

boost::shared_ptr<QWidget> ShowDataTreeNodeBase::createPropertiesWidget() const
{
	return this->getParent()->createPropertiesWidget();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ShowVolumeDataTreeNode::ShowVolumeDataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data) :
	ShowDataTreeNodeBase(repo, data)
{
}
QString ShowVolumeDataTreeNode::getUid() const
{
	return mData->getUid() + "_volume";
}
QString ShowVolumeDataTreeNode::getName() const
{
	return "Volume";
}
QString ShowVolumeDataTreeNode::getType() const
{
	return "data_volume";
}
DataViewProperties ShowVolumeDataTreeNode::getDefiningDataViewProperties() const
{
	return DataViewProperties::createVolume3D();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ShowSlice2DDataTreeNode::ShowSlice2DDataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data) :
	ShowDataTreeNodeBase(repo, data)
{
}
QString ShowSlice2DDataTreeNode::getUid() const
{
	return mData->getUid() + "_slice2D";
}
QString ShowSlice2DDataTreeNode::getName() const
{
	return "Slice 2D";
}
QString ShowSlice2DDataTreeNode::getType() const
{
	return "data_slice2D";
}
DataViewProperties ShowSlice2DDataTreeNode::getDefiningDataViewProperties() const
{
	return DataViewProperties::createSlice2D();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ShowSlice3DDataTreeNode::ShowSlice3DDataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data) :
	ShowDataTreeNodeBase(repo, data)
{
}
QString ShowSlice3DDataTreeNode::getUid() const
{
	return mData->getUid() + "_slice3D";
}
QString ShowSlice3DDataTreeNode::getName() const
{
	return "Slice 3D";
}
QString ShowSlice3DDataTreeNode::getType() const
{
	return "data_slice3D";
}
DataViewProperties ShowSlice3DDataTreeNode::getDefiningDataViewProperties() const
{
	return DataViewProperties::createSlice3D();
}

} // namespace cx

