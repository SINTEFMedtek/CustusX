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

#ifndef CXFRAMEFOREST_H_
#define CXFRAMEFOREST_H_

#include "sscForwardDeclarations.h"

#include <QDomDocument>
#include "sscTypeConversions.h"

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
/**
* \file
* \addtogroup cxServicePatient
* @{
*/

/**
 * \brief A graph combining Space dependencies between all ssc::Data.
 * \ingroup cxServicePatient
 *
 * Relations between coordinate spaces among ssc::Data are created by
 * this class.
 *
 * The graph consists of several directed acyclic graphs.
 *
 *  \date   Sep 23, 2010
 *  \author christiana
 */
class FrameForest
{
public:
	FrameForest();
	QDomNode getNode(QString frame);
	QDomNode getOldestAncestor(QDomNode node);

	QDomNode getOldestAncestorNotCommonToRef(QDomNode child, QDomNode ref);
	std::vector<QDomNode> getDescendantsAndSelf(QDomNode node);
	std::vector<ssc::DataPtr> getDataFromDescendantsAndSelf(QDomNode node);
	QDomDocument getDocument();
private:
	bool isRootNode(QDomNode node);
	QDomNode getNodeAnyway(QString frame);
	bool isAncestorOf(QDomNode node, QDomNode ancestor);
	void insertFrame(ssc::DataPtr data);
	QDomDocument mDocument;
};

/**
* @}
*/
}

#endif /* CXFRAMEFOREST_H_ */
