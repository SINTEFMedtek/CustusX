/*
 * cxFrameForest.h
 *
 *  Created on: Sep 23, 2010
 *      Author: christiana
 */
#ifndef CXFRAMEFOREST_H_
#define CXFRAMEFOREST_H_

#include "sscForwardDeclarations.h"

#include <QDomDocument>
#include "sscTypeConversions.h"
#include "sscData.h"
#include "sscDataManager.h"

/*

<root>
    <frame uid="A">
        <frame uid="MR1">
            <frame uid="S1">
            </frame>
            <frame uid="S2">
            </frame>
        </frame>
        <frame uid="CT1">
        </frame>
    </frame>
    <frame uid="B">
        <frame uid="MR2">
        </frame>
    </frame>
</root>

 */

namespace cx
{

/**A graph combining frame dependencies between all ssc::Data. The graph
 * consists of several directed acyclic graphs.
 *
 */
class FrameForest
{
public:
  FrameForest();
  void insertFrame(ssc::DataPtr data)
  {
      QString parentFrame = qstring_cast(data->getParentFrame());
      QString currentFrame = qstring_cast(data->getUid());

      QDomNode parentNode = this->getNode(parentFrame);
      QDomNode currentNode = this->getNode(currentFrame);

      // move currentNode to child of parentNode
      currentNode = currentNode.parentNode().removeChild(currentNode);
      parentNode.appendChild(currentNode);
  }

  QDomNode getNode(QString frame)
  {
     QDomNodeList list = mDocument.elementsByTagName(frame);
     if (!list.isEmpty())
       return list.item(0);

     QDomElement elem = mDocument.createElement(frame);
     mDocument.documentElement().appendChild(elem);
     return elem;
  }

  //FrameGraphNodePtr getOldestAncestorNotCommonToRef(FrameGraphNodePtr child, FrameGraphNodePtr ref);
  QDomDocument getDocument();
private:
  QDomDocument mDocument;
};

}

#endif /* CXFRAMEFOREST_H_ */
