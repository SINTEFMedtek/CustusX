// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxFrameForest.h"

#include "sscData.h"
#include "sscDataManager.h"

namespace cx
{

/**Create a forest representing all ssc::Data objects and their spatial relationships.
 *
 */
FrameForest::FrameForest()
{
//  std::cout << "Forrest doc2:" << std::endl;
	ssc::DataManager::DataMap allData = ssc::dataManager()->getData();
	mDocument.appendChild(mDocument.createElement("root"));

	for (ssc::DataManager::DataMap::iterator iter = allData.begin(); iter != allData.end(); ++iter)
	{
		this->insertFrame(iter->second);
	}

//  std::cout << "Forrest doc:" << std::endl;
//  std::cout << mDocument.toString(4) << std::endl;
}

QDomDocument FrameForest::getDocument()
{
	return mDocument;
}

/** Insert one data in the correct position in the tree
 */
void FrameForest::insertFrame(ssc::DataPtr data)
{
	QString parentFrame = data->getParentSpace();
	QString currentFrame = data->getSpace();

//    if (parentFrame.isEmpty() || currentFrame.isEmpty())
//    	return;

	QDomNode parentNode = this->getNodeAnyway(parentFrame);
	QDomNode currentNode = this->getNodeAnyway(currentFrame);

	if (parentNode.isNull())
		return;

	// move currentNode to child of parentNode
	currentNode = currentNode.parentNode().removeChild(currentNode);
	parentNode.appendChild(currentNode);
}

/** Given a frame uid, return the QDomNode representing that frame.
 *  Note: Lifetime of retval is controlled by the FrameForest object!
 */
QDomNode FrameForest::getNode(QString frame)
{
	QDomNodeList list = mDocument.elementsByTagName(frame);
	if (list.isEmpty())
		return QDomNode();
	return list.item(0);
}

/** As getNode(), but create the node if if doesn't exist.
 */
QDomNode FrameForest::getNodeAnyway(QString frame)
{
	QDomNode retval = this->getNode(frame);
	if (retval.isNull())
	{
		retval = mDocument.createElement(frame);
		mDocument.documentElement().appendChild(retval);
	}
	return retval;
}

/** Return true if ancestor is an ancestor of node
 */
bool FrameForest::isAncestorOf(QDomNode node, QDomNode ancestor)
{
	//std::cout << "isAncestorOf : " << node.toElement().tagName() << "---"  << ancestor.toElement().tagName() << std::endl;

	while (!this->isRootNode(node))
	{
		if (node == ancestor)
		{
			//std::cout << "return true" << std::endl;;
			return true;
		}
		node = node.parentNode();
	}
	//std::cout << "return false" << std::endl;;
	return false;
}

bool FrameForest::isRootNode(QDomNode node)
{
	return node == mDocument.documentElement();
}

/** Find the oldest ancestor of node.
 */
QDomNode FrameForest::getOldestAncestor(QDomNode node)
{
	//while (!node.parentNode().isNull() && !node.parentNode().toElement().tagName().isEmpty())
	while (!this->isRootNode(node.parentNode()))
	{
		node = node.parentNode();
	}
	return node;
}

/** Find the oldest ancestor of child, that is not also an ancestor of ref.
 */
QDomNode FrameForest::getOldestAncestorNotCommonToRef(QDomNode node, QDomNode ref)
{
	//std::cout << "getOldestAncestorNotCommonToRef " << node.toElement().tagName() << " - " << ref.toElement().tagName() << std::endl;
	if (this->isAncestorOf(ref, node))
		return QDomNode();

	while (!this->isRootNode(node.parentNode()))
	{
		//std::cout << "getOldestAncestorNotCommonToRef iterate start: " << node.toElement().tagName()  << std::endl;
		if (this->isAncestorOf(ref, node.parentNode()))
			break;
		node = node.parentNode();
		//std::cout << "getOldestAncestorNotCommonToRef iterate stop: " << node.toElement().tagName()  << std::endl;
	}
	//std::cout << std::endl;
	return node;
}

/** Return the node and all its children recursively in one flat vector.
 */
std::vector<QDomNode> FrameForest::getDescendantsAndSelf(QDomNode node)
{
	std::vector<QDomNode> retval;
	retval.push_back(node);

	for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling())
	{
		std::vector<QDomNode> subnodes = this->getDescendantsAndSelf(child);
		std::copy(subnodes.begin(), subnodes.end(), back_inserter(retval));
	}
	return retval;
}

/** As getAllNodesFor(QDomNode), but return the nodes as data objects.
 *  Those frames not representing data are discarded.
 */
std::vector<ssc::DataPtr> FrameForest::getDataFromDescendantsAndSelf(QDomNode node)
{
	std::vector<QDomNode> nodes = this->getDescendantsAndSelf(node);
	std::vector<ssc::DataPtr> retval;

	for (unsigned i = 0; i < nodes.size(); ++i)
	{
		ssc::DataPtr data = ssc::dataManager()->getData(nodes[i].toElement().tagName());
		if (data)
			retval.push_back(data);
	}
	return retval;
}

}
