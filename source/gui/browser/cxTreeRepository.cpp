/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTreeRepository.h"
#include "cxPatientModelService.h"
#include "cxDefinitions.h"
#include "cxData.h"
#include "cxTreeNode.h"
#include "cxDataTreeNode.h"
#include "cxSpaceTreeNode.h"
#include "cxGroupTreeNode.h"
#include "cxTopTreeNode.h"
#include "cxToolTreeNode.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxActiveData.h"
#include "cxVisServices.h"
#include "cxShowDataTreeNode.h"
#include "cxViewServiceTreeNode.h"
#include "cxViewGroupTreeNode.h"
#include "cxViewService.h"
#include "cxStringListProperty.h"
#include "cxStringProperty.h"

namespace cx
{

boost::shared_ptr<QWidget> WidgetTypeRepository::findMetricWidget(DataPtr data)
{
	for (unsigned i=0; i<mWidgets.size(); ++i)
	{
		boost::shared_ptr<SingleMetricWidget> w = boost::dynamic_pointer_cast<SingleMetricWidget>(mWidgets[i]);
		if(w && w->getData() && data && w->getData()->getUid() == data->getUid())
			return w;
	}
	return boost::shared_ptr<QWidget>();
}

void WidgetTypeRepository::add(boost::shared_ptr<QWidget> widget)
{
	mWidgets.push_back(widget);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TreeRepositoryPtr TreeRepository::create(XmlOptionFile options, VisServicesPtr services)
{
	TreeRepositoryPtr retval(new TreeRepository(options, services));
	retval->mSelf = retval;
	retval->insertTopNode();
	return retval;
}

TreeRepository::TreeRepository(XmlOptionFile options, VisServicesPtr services) :
	mInvalid(true),
	mServices(services),
	mOptions(options)
{
	this->createVisibilityProperty();
	this->createModeProperty();

	mWidgetTypeRepository.reset(new WidgetTypeRepository());

	this->startListen();
}

void TreeRepository::createVisibilityProperty()
{
	QStringList allNodeTypes = QStringList() << "data" << "metric" << "image" << "model" << "tool" << "view";
	mVisibilityProperty = StringListProperty::initialize("visible_node_types",
														"Visibility",
														"Select visible node types",
														allNodeTypes,
														allNodeTypes,
														 mOptions.getElement());
	connect(mVisibilityProperty.get(), &StringListProperty::changed, this, &TreeRepository::invalidate);
}

void TreeRepository::createModeProperty()
{
	QStringList allModes = QStringList() << "spaces" << "flat";

	mModeProperty = StringProperty::initialize("mode",
											   "mode",
											   "Node display mode",
											   allModes.front(),
											   allModes,
											   mOptions.getElement());
	connect(mModeProperty.get(), &Property::changed, this, &TreeRepository::loaded);
}

TreeRepository::~TreeRepository()
{
	this->stopListen();
}

void TreeRepository::update()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeRepository::update(), invalid=" << mInvalid;
	if (mInvalid)
		this->rebuild();
	mInvalid = true;
}

void TreeRepository::invalidate()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeRepository::invalidate()";
	mInvalid = true;
	emit invalidated();
}

void TreeRepository::startListen()
{
	connect(this->getServices()->patient().get(), &PatientModelService::patientChanged, this, &TreeRepository::loaded);
	connect(this->getServices()->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &TreeRepository::invalidate);
	connect(this->getServices()->tracking().get(), &TrackingService::stateChanged, this, &TreeRepository::loaded);
	connect(this->getServices()->patient()->getActiveData().get(), &ActiveData::activeDataChanged, this, &TreeRepository::onChanged);
}

void TreeRepository::stopListen()
{
	disconnect(this->getServices()->patient().get(), &PatientModelService::patientChanged, this, &TreeRepository::loaded);
	disconnect(this->getServices()->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &TreeRepository::invalidate);
	disconnect(this->getServices()->tracking().get(), &TrackingService::stateChanged, this, &TreeRepository::loaded);
	disconnect(this->getServices()->patient()->getActiveData().get(), &ActiveData::activeDataChanged, this, &TreeRepository::onChanged);
}

std::vector<TreeNodePtr> TreeRepository::getNodes()
{
	return mNodes;
}

TreeNodePtr TreeRepository::getNode(QString uid)
{
	for (unsigned i=0; i<mNodes.size(); ++i)
	{
		if (mNodes[i]->getUid()==uid)
			return mNodes[i];
	}
	return TreeNodePtr();
}

TreeNodePtr TreeRepository::getTopNode()
{
	std::vector<TreeNodePtr> all = this->getNodes();
	for (unsigned i=0; i<all.size(); ++i)
		if (all[i]->getUid() == "node::invisible_top")
			return all[i];
	CX_LOG_CHANNEL_DEBUG("CA") << "invalid tree - did not find top node";
	return TreeNodePtr();
}

VisServicesPtr TreeRepository::getServices()
{
	return mServices;
}

WidgetTypeRepositoryPtr TreeRepository::getWidgetTypeRepository()
{
	return mWidgetTypeRepository;
}

QString TreeRepository::getMode() const
{
	return mModeProperty->getValue();
}

QStringList TreeRepository::getVisibleNodeTypes() const
{
	return mVisibilityProperty->getValue();
}

QStringList TreeRepository::getAllNodeTypes() const
{
	return mVisibilityProperty->getValueRange();
}

void TreeRepository::insertTopNode()
{
	//	TreeNodePtr topnode(new TopTreeNode(mSelf));
//	mNodes.push_back(topnode);
	this->appendNode(new TopTreeNode(mSelf));
//	CX_LOG_CHANNEL_DEBUG("CA") << "  - built topnode";
}

void TreeRepository::rebuild()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "  reuild tree...";
	for (unsigned i=0; i<mNodes.size(); ++i)
		disconnect(mNodes[i].get(), &TreeNode::changed, this, &TreeRepository::onChanged);
	mNodes.clear();

	this->insertTopNode();

	QStringList groups = QStringList() << "tool" << "data" << "space" << "view";
	for (unsigned i=0; i<groups.size(); ++i)
		this->insertGroupNode(groups[i]);

	this->insertSpaceNode(CoordinateSystem(csREF));

	std::map<QString, DataPtr> source = this->getServices()->patient()->getDatas();
	for (std::map<QString, DataPtr>::const_iterator iter = source.begin(); iter != source.end(); ++iter)
	{
		this->insertDataNode(iter->second);
	}
	for (std::map<QString, DataPtr>::const_iterator iter = source.begin(); iter != source.end(); ++iter)
	{
		QString space = iter->second->getParentSpace();
		if (space.isEmpty())
			continue;
		if (source.count(space))
			continue;
		this->insertSpaceNode(CoordinateSystem(csDATA, space));
	}

	this->insertSpaceNode(CoordinateSystem(csPATIENTREF));

	std::map<QString, ToolPtr> tools = this->getServices()->tracking()->getTools();
	for (std::map<QString, ToolPtr>::const_iterator iter = tools.begin(); iter != tools.end(); ++iter)
	{
		this->insertToolNode(iter->second);
	}

	for (unsigned i=0; i<this->getServices()->view()->groupCount(); ++i)
	{
		this->appendNode(new ViewGroupTreeNode(mSelf, i));
	}

//	for (unsigned i=0; i<mNodes.size(); ++i)
//		CX_LOG_CHANNEL_DEBUG("CA") << "  node: " << mNodes[i]->getUid();
}

void TreeRepository::insertGroupNode(QString groupname)
{
	if (this->getNodeForGroup(groupname))
		return;
	if (groupname=="view")
		this->appendNode(new ViewServiceTreeNode(mSelf));
	else
		this->appendNode(new GroupTreeNode(mSelf, groupname));
}

TreeNodePtr TreeRepository::getNodeForGroup(QString groupname)
{
	return this->getNode(QString("group_%1").arg(groupname));
}

void TreeRepository::insertToolNode(ToolPtr tool)
{
	if (this->getNode(tool->getUid()))
		return;
	this->appendNode(new ToolTreeNode(mSelf, tool));
}

void TreeRepository::insertDataNode(DataPtr data)
{
	if (this->getNode(data->getUid()))
		return;

	this->appendNode(new DataTreeNode(mSelf, data));

	this->appendNode(new ShowVolumeDataTreeNode(mSelf, data));
	this->appendNode(new ShowSlice2DDataTreeNode(mSelf, data));
	this->appendNode(new ShowSlice3DDataTreeNode(mSelf, data));
}

void TreeRepository::appendNode(TreeNode* rawNode)
{
	TreeNodePtr bnode(rawNode);
	TreeNodePtr node(new CachedTreeNode(bnode));

	if (!this->getNode(node->getUid()))
	{
		// TODO need a more detailed change policy: invalidate only when parent/childe
		// structure is changed.
//		connect(node.get(), &TreeNode::changed, this, &TreeRepository::invalidated);
		connect(node.get(), &TreeNode::changed, this, &TreeRepository::onChanged);
		mNodes.push_back(node);
	}
}

void TreeRepository::onChanged()
{
	TreeNode* node = dynamic_cast<TreeNode*>(sender());
//	CX_LOG_CHANNEL_DEBUG("CA") << "   TreeRepository::onChanged() node=" << ((node)?node->getName():"-");
	emit changed(node);
}

void TreeRepository::insertSpaceNode(CoordinateSystem space)
{
	if (this->getNode(space.toString()) || this->getNode(space.mRefObject))
		return;
	this->appendNode(new SpaceTreeNode(mSelf, space));
}


} // namespace cx
