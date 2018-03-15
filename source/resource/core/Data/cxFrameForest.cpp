/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFrameForest.h"

#include "cxData.h"

namespace cx
{

/**Create a forest representing all Data objects and their spatial relationships.
 *
 */
FrameForest::FrameForest(const std::map<QString, DataPtr> &source) : mSource(source)
{
//  std::cout << "Forrest doc2:" << std::endl;
//	DataManager::DataMap allData = mDataManager->getData();
	mDocument.appendChild(mDocument.createElement("root"));

	for (std::map<QString, DataPtr>::const_iterator iter = source.begin(); iter != source.end(); ++iter)
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
void FrameForest::insertFrame(DataPtr data)
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
	if (this->isRootNode(node))
		return node;
	while (!this->isRootNode(node.parentNode()))
		node = node.parentNode();
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
std::vector<DataPtr> FrameForest::getDataFromDescendantsAndSelf(QDomNode node)
{
	std::vector<QDomNode> nodes = this->getDescendantsAndSelf(node);
	std::vector<DataPtr> retval;

	for (unsigned i = 0; i < nodes.size(); ++i)
	{
		DataPtr data = mSource[nodes[i].toElement().tagName()];
//		DataPtr data = mDataManager->getData(nodes[i].toElement().tagName());
		if (data)
			retval.push_back(data);
	}
	return retval;
}

}
