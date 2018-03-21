/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxMetricUtilities.h"
#include "cxNullDeleter.h"

namespace cx
{


DataTreeNode::DataTreeNode(TreeRepositoryWeakPtr repo, DataPtr data) :
	TreeNodeImpl(repo), mData(data)
{
	// too expensive: this happens for every position change in some cases (metrics),
	// while nothing is changed in the gui.
//	connect(mData.get(), &Data::transformChanged, this, &TreeNode::changed);
}

DataTreeNode::~DataTreeNode()
{
//	disconnect(mData.get(), &Data::transformChanged, this, &TreeNode::changed);
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

boost::shared_ptr<QWidget> DataTreeNode::createPropertiesWidget() const
{
	WidgetTypeRepositoryPtr wrepo = this->repo()->getWidgetTypeRepository();

	if (boost::dynamic_pointer_cast<Mesh>(mData))
	{
		boost::shared_ptr<AllMeshPropertiesWidget> widget = wrepo->find<AllMeshPropertiesWidget>();
		if (!widget)
		{
			StringPropertySelectMeshPtr meshSelector = StringPropertySelectMesh::New(this->getServices()->patient());
			widget.reset( new AllMeshPropertiesWidget(meshSelector,
											 this->getServices(),
											 NULL));
			wrepo->add(widget);
		}
		widget->getSelector()->setValue(mData->getUid());
		return widget;
	}
	if (boost::dynamic_pointer_cast<Image>(mData))
	{
		boost::shared_ptr<ImagePropertiesWidget> widget = wrepo->find<ImagePropertiesWidget>();
		if (!widget)
		{
			widget.reset (new ImagePropertiesWidget(this->getServices(), NULL));
			wrepo->add(widget);
		}
		return widget;
	}
	if(boost::dynamic_pointer_cast<DataMetric>(mData))
	{
		boost::shared_ptr<QWidget> widget = wrepo->findMetricWidget(mData);
		if(!widget)
		{
			MetricUtilities utilities(this->getServices());
			widget.reset(utilities.createMetricWidget(mData));
			wrepo->add(widget);
		}
		return widget;

	}
	return boost::shared_ptr<QWidget>(new QLabel(QString("Data widget %1 ").arg(mData->getName())));
}


} // namespace cx
