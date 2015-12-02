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

