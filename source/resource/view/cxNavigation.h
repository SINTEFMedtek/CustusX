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
#ifndef CXNAVIGATION_H
#define CXNAVIGATION_H

#include "cxResourceVisualizationExport.h"

#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxViewGroupData.h"

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/** Functions for navigating in the visualization scene(s).
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT Navigation
{
public:
	enum VIEW_TYPE { v2D = 0x01, v3D=0x02, vBOTH=0x03 };
	Navigation(VisServicesPtr services, CameraControlPtr camera3D=CameraControlPtr());
	void centerToTooltip();
	void centerToPosition(Vector3D p_r, QFlags<VIEW_TYPE> viewType=vBOTH);

	void centerToDataInActiveViewGroup(DataViewProperties properties=DataViewProperties::createFull());
	void centerToDataInViewGroup(ViewGroupDataPtr group, DataViewProperties properties=DataViewProperties::createFull());
private:
	void moveManualToolToPosition(Vector3D& p_r);
	Vector3D findDataCenter(const std::vector<DataPtr> &data);
	void centerToData(DataPtr image);
	void centerToData(const std::vector<DataPtr>& images);

	VisServicesPtr mServices;
	CameraControlPtr mCamera3D;
};


} // namespace cx

#endif // CXNAVIGATION_H
