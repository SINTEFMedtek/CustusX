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
* \addtogroup cx_service_patient
* @{
*/

/**
 * \brief A graph combining Space dependencies between all Data.
 * \ingroup cx_service_patient
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
