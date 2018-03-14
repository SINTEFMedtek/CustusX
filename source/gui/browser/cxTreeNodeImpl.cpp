/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTreeNodeImpl.h"
#include "cxTreeRepository.h"
#include "cxLogger.h"
#include <QIcon>
#include <QPainter>


namespace cx
{


TreeNodeImpl::TreeNodeImpl(TreeRepositoryWeakPtr repo) :
	mRepository(repo)
{

}

std::vector<TreeNodePtr> TreeNodeImpl::getChildren() const
{
	// implement as inverse of getParent()
	std::vector<TreeNodePtr> retval;
	std::vector<TreeNodePtr> all = this->repo()->getNodes();
	for (unsigned i=0; i<all.size(); ++i)
	{
		TreeNodePtr p = all[i]->getParent();
		if (p && (p->getUid() == this->getUid()))
			retval.push_back(all[i]);
//	CX_LOG_CHANNEL_DEBUG("CA") << QString("  - data node %1 children: %2").arg(this->getName()).arg(retval.size());
	}
	return retval;
}

//TreeNodePtr TreeNodeImpl::getParent() const
//{
//	if (!this->isVisibleNode())
//		return TreeNodePtr();
//	CX_LOG_CHANNEL_DEBUG("CA") << "===== TreeNodeImpl::getParent() " << this->getName();
//	TreeNodePtr parent = this->getParentIgnoreVisiblity();
//	CX_LOG_CHANNEL_DEBUG("CA") << "    parent=  " << ((parent.get()!=NULL)?parent->getName():"NULL");
//	while (parent && !parent->isVisibleNode())
//	{
//		parent = parent->getParent();
//		CX_LOG_CHANNEL_DEBUG("CA") << "      parent=  " << ((parent.get()!=NULL)?parent->getName():"NULL");
//	}
//	CX_LOG_CHANNEL_DEBUG("CA") << "    parent end=  " << ((parent.get()!=NULL)?parent->getName():"NULL");
//	return parent;
////	return this->walkToFirstVisibleParentIncludingSelf(parent);
//}

std::vector<TreeNodePtr> TreeNodeImpl::getVisibleChildren() const
{
	// implement as inverse of getParent()
	std::vector<TreeNodePtr> retval;
	std::vector<TreeNodePtr> all = this->repo()->getNodes();
	for (unsigned i=0; i<all.size(); ++i)
	{
		TreeNodePtr vp = all[i]->getVisibleParent();
		if (vp && (vp->getUid() == this->getUid()))
			retval.push_back(all[i]);
	}
	return retval;
}

TreeNodePtr TreeNodeImpl::getVisibleParent() const
{
	if (!this->isVisibleNode())
		return TreeNodePtr();
	TreeNodePtr parent = this->getParent();
	while (parent && !parent->isVisibleNode())
	{
		parent = parent->getParent();
	}
	return parent;
}


bool TreeNodeImpl::isVisibleNode() const
{
	bool isKnown = this->repo()->getAllNodeTypes().contains(this->getType());
	bool isVisible = this->repo()->getVisibleNodeTypes().contains(this->getType());
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeNodeImpl::isVisibleNode() name=" << this->getName()
//							   << " isKnown=" << isKnown
//							   << " isVisible=" << isVisible;
	return !isKnown || isVisible;
}

VisServicesPtr TreeNodeImpl::getServices() const
{
	return this->repo()->getServices();
}

TreeRepositoryPtr TreeNodeImpl::repo()
{
	return mRepository.lock();
}

const TreeRepositoryPtr TreeNodeImpl::repo() const
{
	return mRepository.lock();
}

QIcon TreeNodeImpl::addBackgroundColorToIcon(QIcon input, QColor color) const
{
	QPixmap org = input.pixmap(input.actualSize(QSize(1000,1000)));
	QImage image(org.size(), QImage::Format_RGBA8888);
	QPainter painter(&image);
	painter.fillRect(image.rect(), color);
	painter.drawPixmap(0,0,org);
	return QIcon(QPixmap::fromImage(image));
}

} // namespace cx

