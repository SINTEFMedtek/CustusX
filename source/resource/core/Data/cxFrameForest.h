/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFRAMEFOREST_H_
#define CXFRAMEFOREST_H_

#include "cxResourceExport.h"

#include "cxForwardDeclarations.h"

#include <QDomDocument>
#include "cxTypeConversions.h"

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
* \addtogroup cx_resource_core_data
* @{
*/

/**
 * \brief A graph combining Space dependencies between all Data.
 * \ingroup cx_resource_core_data
 *
 * Relations between coordinate spaces among Data are created by
 * this class.
 *
 * The graph consists of several directed acyclic graphs.
 *
 *  \date   Sep 23, 2010
 *  \author christiana
 */
class cxResource_EXPORT FrameForest
{
public:
	explicit FrameForest(const std::map<QString, DataPtr>& source);
	QDomNode getNode(QString frame);
	QDomNode getOldestAncestor(QDomNode node);

	QDomNode getOldestAncestorNotCommonToRef(QDomNode child, QDomNode ref);
	std::vector<QDomNode> getDescendantsAndSelf(QDomNode node);
	std::vector<DataPtr> getDataFromDescendantsAndSelf(QDomNode node);
	QDomDocument getDocument();
private:
	bool isRootNode(QDomNode node);
	QDomNode getNodeAnyway(QString frame);
	bool isAncestorOf(QDomNode node, QDomNode ancestor);
	void insertFrame(DataPtr data);
	QDomDocument mDocument;

	std::map<QString, DataPtr> mSource;
};

/**
* @}
*/
}

#endif /* CXFRAMEFOREST_H_ */
