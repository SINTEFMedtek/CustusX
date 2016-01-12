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
#ifndef CXSPACEPROVIDERIMPL_H
#define CXSPACEPROVIDERIMPL_H

#include "cxResourceExport.h"

#include "cxSpaceProvider.h"
#include "cxForwardDeclarations.h"

namespace cx
{

/** Provides information about all the coordinate systems in the application.
 *
 *
 * \ingroup cx_resource_core_utilities
 * \date 2014-02-21
 * \author christiana
 */
class cxResource_EXPORT SpaceProviderImpl : public SpaceProvider
{
public:
	SpaceProviderImpl(TrackingServicePtr trackingService, PatientModelServicePtr dataManager);
	virtual ~SpaceProviderImpl() {}

	virtual Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to); ///< to_M_from
	virtual std::vector<CoordinateSystem> getSpacesToPresentInGUI();
	virtual std::map<QString, QString> getDisplayNamesForCoordRefObjects();
	virtual SpaceListenerPtr createListener();

   virtual Vector3D getActiveToolTipPoint(CoordinateSystem to, bool useOffset = false); ///< P_to, active tools current point in coord
	virtual Transform3D getActiveToolTipTransform(CoordinateSystem to, bool useOffset = false);///< Get toMt, where t is active tool
	virtual Transform3D get_rMpr();

	virtual CoordinateSystem getS(ToolPtr tool); ///<tools sensor coordinate system
	virtual CoordinateSystem getT(ToolPtr tool); ///<tools coordinate system
	virtual CoordinateSystem getTO(ToolPtr tool); ///<tool offset coordinate system
	virtual CoordinateSystem getD(DataPtr data);///<datas coordinate system	static CoordinateSystem getPr(); ///<patient references coordinate system
	virtual CoordinateSystem getPr();
	virtual CoordinateSystem getR(); ///<data references coordinate system
	virtual CoordinateSystem convertToSpecific(CoordinateSystem space);

private:
	Transform3D get_rMfrom(CoordinateSystem from); ///< ref_M_from

	Transform3D get_rMr(); ///< ref_M_ref
	Transform3D get_rMd(QString uid);
	Transform3D get_rMdv(QString uid);
//	Transform3D get_rMpr();
	Transform3D get_rMt(QString uid);
	Transform3D get_rMto(QString uid);
	Transform3D get_rMs(QString uid);

	CoordinateSystem getToolCoordinateSystem(ToolPtr tool);

	TrackingServicePtr mTrackingService;
	PatientModelServicePtr mDataManager;
};

} // namespace cx

#endif // CXSPACEPROVIDERIMPL_H
