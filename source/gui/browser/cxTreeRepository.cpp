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
#include "cxTreeRepository.h"
#include "cxLegacySingletons.h"
#include "cxPatientModelService.h"
#include "cxDefinitions.h"
#include "cxData.h"
#include "cxTreeNode.h"
#include "cxDataTreeNode.h"
#include "cxSpaceTreeNode.h"
#include "cxTopTreeNode.h"
#include "cxLogger.h"

namespace cx
{

TreeRepositoryPtr TreeRepository::create()
{
	TreeRepositoryPtr retval(new TreeRepository());
	retval->mSelf = retval;
	retval->rebuild();
	CX_LOG_CHANNEL_DEBUG("CA") << "  - create repo: " << retval.get();
	return retval;
}

TreeRepository::TreeRepository()
{
}

TreeRepository::~TreeRepository()
{
	CX_LOG_CHANNEL_DEBUG("CA") << "  - destroy repo: " << this;
}

std::vector<TreeNodePtr> TreeRepository::getNodes()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeRepository::getNodes() " << mNodes.size();
	return mNodes;
}

TreeNodePtr TreeRepository::getNode(QString uid)
{
	for (unsigned i=0; i<mNodes.size(); ++i)
	{
		if (mNodes[i]->getUid()==uid)
			return mNodes[i];
	}

//	CX_LOG_CHANNEL_DEBUG("CA") << "search failed - did not find uid="+uid;
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

void TreeRepository::rebuild()
{
	mNodes.clear();
	TreeNodePtr topnode(new TopTreeNode(mSelf));
	mNodes.push_back(topnode);
	CX_LOG_CHANNEL_DEBUG("CA") << "  - built topnode";

	std::map<QString, DataPtr> source = patientService()->getData();
	for (std::map<QString, DataPtr>::const_iterator iter = source.begin(); iter != source.end(); ++iter)
	{
		this->insertDataNode(iter->second);
	}
	CX_LOG_CHANNEL_DEBUG("CA") << "  - built datanodes";
	for (std::map<QString, DataPtr>::const_iterator iter = source.begin(); iter != source.end(); ++iter)
	{
		QString space = iter->second->getParentSpace();
		if (source.count(space))
			continue;
		this->insertSpaceNode(CoordinateSystem(csDATA, space));
	}
	CX_LOG_CHANNEL_DEBUG("CA") << "  - built spacenodes";

	this->insertSpaceNode(CoordinateSystem(csREF));
	CX_LOG_CHANNEL_DEBUG("CA") << "  - built refspacenode";
}

void TreeRepository::insertDataNode(DataPtr data)
{
	if (this->getNode(data->getUid()))
		return;

	TreeNodePtr node(new DataTreeNode(mSelf, data));
	mNodes.push_back(node);
}

void TreeRepository::insertSpaceNode(CoordinateSystem space)
{
	if (this->getNode(space.toString()) || this->getNode(space.mRefObject))
		return;

	TreeNodePtr node(new SpaceTreeNode(mSelf, space));
	mNodes.push_back(node);
}

} // namespace cx
