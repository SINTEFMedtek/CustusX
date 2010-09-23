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
  QDomNode getNode(QString frame);
  QDomNode getOldestAncestor(QDomNode node);

  QDomNode getOldestAncestorNotCommonToRef(QDomNode child, QDomNode ref);
  std::vector<QDomNode> getAllNodesFor(QDomNode node);
  std::vector<ssc::DataPtr> getAllDataIn(QDomNode node);
  QDomDocument getDocument();
private:
  QDomNode getNodeAnyway(QString frame);
  bool isAncestorOf(QDomNode node, QDomNode ancestor);
  void insertFrame(ssc::DataPtr data);
  QDomDocument mDocument;
};

}

#endif /* CXFRAMEFOREST_H_ */
