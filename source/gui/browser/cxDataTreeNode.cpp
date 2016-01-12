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
#include <QLabel>
#include "cxMesh.h"
#include "cxMeshInfoWidget.h"
#include "cxImagePropertiesWidget.h"
#include "cxImage.h"
#include "cxSelectDataStringProperty.h"
#include "cxDataMetric.h"

namespace cx
{


DataTreeNode::DataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data) :
	TreeNodeImpl(repo), mData(data)
{
	connect(mData.get(), &Data::transformChanged, this, &TreeNode::changed);
}

DataTreeNode::~DataTreeNode()
{
	disconnect(mData.get(), &Data::transformChanged, this, &TreeNode::changed);
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

bool DataTreeNode::isVisibleNode() const
{
	QStringList visible = this->repo()->getVisibleNodeTypes();

	bool hasData = visible.contains(this->getType());
	if (!hasData)
		return false;

	if (boost::dynamic_pointer_cast<Mesh>(mData) && !visible.contains("model"))
		return false;
	if (boost::dynamic_pointer_cast<Image>(mData) && !visible.contains("image"))
		return false;
	if (boost::dynamic_pointer_cast<DataMetric>(mData) && !visible.contains("metric"))
		return false;

	return true;
}

TreeNodePtr DataTreeNode::getParent() const
{
	if (this->repo()->getMode()=="flat")
		return this->repo()->getNodeForGroup("data");

	if (mData->getParentSpace().isEmpty())
		return this->repo()->getNode(CoordinateSystem(csREF).toString());
	TreeNodePtr parent = this->repo()->getNode(mData->getParentSpace());
	if (!parent)
		parent = this->repo()->getNode(CoordinateSystem(csDATA, mData->getParentSpace()).toString());
	return parent;
}

void DataTreeNode::activate()
{
	this->getServices()->patient()->getActiveData()->setActive(mData);
}

QIcon DataTreeNode::getIcon() const
{
	DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(mData);
	if (metric)
		return this->addBackgroundColorToIcon(mData->getIcon(), metric->getColor());
	return mData->getIcon();
}

QVariant DataTreeNode::getColor() const
{
	DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(mData);
	if (metric)
		return metric->getColor();
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(mData);
	if (mesh)
		return mesh->getColor();
	return QVariant();
//	return QColor("black");
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

bool DataTreeNode::isDefaultExpanded() const
{
	return (this->repo()->getMode()!="flat");
}

void DataTreeNode::remove()
{
	this->getServices()->patient()->removeData(mData->getUid());
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

QWidget* DataTreeNode::createPropertiesWidget() const
{
	WidgetTypeRepositoryPtr wrepo = this->repo()->getWidgetTypeRepository();

	if (boost::dynamic_pointer_cast<Mesh>(mData))
	{
		MeshInfoWidget* widget = wrepo->find<MeshInfoWidget>();
		if (!widget)
		{
			StringPropertySelectMeshPtr meshSelector = StringPropertySelectMesh::New(this->getServices()->patient());
			widget = new MeshInfoWidget(meshSelector,
										this->getServices()->patient(),
										this->getServices()->view(),
										NULL);
			wrepo->add(widget);
		}
		widget->getSelector()->setValue(mData->getUid());
		return widget;
	}
	if (boost::dynamic_pointer_cast<Image>(mData))
	{
		ImagePropertiesWidget* widget = wrepo->find<ImagePropertiesWidget>();
		if (!widget)
		{
			widget = new ImagePropertiesWidget(this->getServices(), NULL);
			wrepo->add(widget);
		}
		return widget;
	}
	return new QLabel(QString("Data widget %1 ").arg(mData->getName()));
}


} // namespace cx
