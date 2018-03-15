/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

