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

#ifndef CXDOMINANTTOOLPROXY_H_
#define CXDOMINANTTOOLPROXY_H_

#include "cxResourceExport.h"
#include "cxTool.h"

namespace cx
{

/**
 * \file
 * \addtogroup cx_resource_core_tool
 * @{
 */

typedef boost::shared_ptr<class DominantToolProxy> DominantToolProxyPtr;
class ToolManager;

/**
 * \brief Helper class for connecting to the dominant tool.
 * \ingroup cx_service_tracking
 *
 * By listening to this class, you will always listen
 * to the dominant tool.
 *
 *  \date Sep 9, 2011
 *  \author christiana
 */
class cxResource_EXPORT DominantToolProxy: public QObject
{
Q_OBJECT

public:
	static DominantToolProxyPtr New(TrackingServiceOldPtr toolManager)
	{
		return DominantToolProxyPtr(new DominantToolProxy(toolManager));
	}

	ToolPtr getTool() { return mTool; }

signals:
	// the original tool change signal
	void dominantToolChanged(const QString&);

	// forwarding of dominant tool signals
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void toolVisible(bool visible);
	void tooltipOffset(double offset);
	void toolProbeSector();
	void tps(int);


private slots:
	void dominantToolChangedSlot(const QString&);
private:
	DominantToolProxy(TrackingServiceOldPtr toolManager);
	ToolPtr mTool;
	TrackingServiceOldPtr mToolManager;
};

/**
 * @}
 */
}

#endif /* CXDOMINANTTOOLPROXY_H_ */
