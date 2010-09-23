/*
 * cxFrameForest.cpp
 *
 *  Created on: Sep 23, 2010
 *      Author: christiana
 */

#include "cxFrameForest.h"

namespace cx
{

/**Create a forest representing all ssc::Data objects and their spatial relationships.
 *
 */
FrameForest::FrameForest()
{
  //std::cout << "Forrest doc2:" << std::endl;
  ssc::DataManager::DataMap allData = ssc::dataManager()->getData();
  mDocument.appendChild(mDocument.createElement("root"));

  for (ssc::DataManager::DataMap::iterator iter=allData.begin(); iter!=allData.end(); ++iter)
  {
    this->insertFrame(iter->second);
  }

  std::cout << "Forrest doc:" << std::endl;
  std::cout << mDocument.toString(4) << std::endl;
}

/** Insert one data in the correct position in the tree
 */
void FrameForest::insertFrame(ssc::DataPtr data)
{
    QString parentFrame = qstring_cast(data->getParentFrame());
    QString currentFrame = qstring_cast(data->getUid());

    QDomNode parentNode = this->getNodeAnyway(parentFrame);
    QDomNode currentNode = this->getNodeAnyway(currentFrame);

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
    QDomElement retval = mDocument.createElement(frame);
    mDocument.documentElement().appendChild(retval);
  }
  return retval;
}

/** Return true if ancestor is an ancestor of node
 */
bool FrameForest::isAncestorOf(QDomNode node, QDomNode ancestor)
{
  while (!node.isNull())
  {
    if (node==ancestor)
      return true;
    node = node.parentNode();
  }
  return false;
}

/** Find the oldest ancestor of node.
 */
QDomNode FrameForest::getOldestAncestor(QDomNode node)
{
  while (!node.parentNode().isNull())
  {
    node = node.parentNode();
  }
  return node;
}

/** Find the oldest ancestor of child, that is not also an ancestor of ref.
 */
QDomNode FrameForest::getOldestAncestorNotCommonToRef(QDomNode node, QDomNode ref)
{
  if (this->isAncestorOf(node, ref))
    return QDomNode();

  while (!node.parentNode().isNull())
  {
    if (this->isAncestorOf(node.parentNode(), ref))
      break;
    node = node.parentNode();
  }
  return node;
}

/** Return the node and all its children recursively in one flat vector.
 */
std::vector<QDomNode> FrameForest::getAllNodesFor(QDomNode node)
{
  std::vector<QDomNode> retval;
  retval.push_back(node);

  for (QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling())
  {
    std::vector<QDomNode> subnodes = this->getAllNodesFor(child);
    std::copy(subnodes.begin(), subnodes.end(), back_inserter(retval));
  }
  return retval;
}

/** As getAllNodesFor(QDomNode), but return the nodes as data objects.
 *  Those frames not representing data are discarded.
 */
std::vector<ssc::DataPtr> FrameForest::getAllDataIn(QDomNode node)
{
  std::vector<QDomNode> nodes = this->getAllNodesFor(node);
  std::vector<ssc::DataPtr> retval;

  for (unsigned i=0; i<nodes.size(); ++i)
  {
    ssc::DataPtr data = ssc::dataManager()->getData(string_cast(nodes[i].toElement().tagName()));
    if (data)
      retval.push_back(data);
  }
  return retval;
}


}
