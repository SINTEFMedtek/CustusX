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
#ifndef CXSPACEPROVIDER_H
#define CXSPACEPROVIDER_H

#include "cxResourceExport.h"

#include "cxTransform3D.h"
#include "cxCoordinateSystemHelpers.h"
#include <QObject>

namespace cx
{
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

/** Provides information about all the coordinate systems in the application.
 *
 *
 * \ingroup cx_resource_core_utilities
 * \date 2014-02-21
 * \author christiana
 */
class cxResource_EXPORT SpaceProvider : public QObject
{
	Q_OBJECT
public:
	virtual ~SpaceProvider() {}

	virtual Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to) = 0; ///< to_M_from
	virtual std::vector<CoordinateSystem> getSpacesToPresentInGUI() = 0;
	virtual std::map<QString, QString> getDisplayNamesForCoordRefObjects() = 0;
	virtual SpaceListenerPtr createListener() = 0;

	virtual Vector3D getActiveToolTipPoint(CoordinateSystem to, bool useOffset = false) = 0; ///< P_to, active tools current point in coord
	virtual Transform3D getActiveToolTipTransform(CoordinateSystem to, bool useOffset = false) = 0;///< Get toMt, where t is active tool
	virtual Transform3D get_rMpr() = 0;

	virtual CoordinateSystem getS(ToolPtr tool) = 0; ///<tools sensor coordinate system
	virtual CoordinateSystem getT(ToolPtr tool) = 0; ///<tools coordinate system
	virtual CoordinateSystem getTO(ToolPtr tool) = 0; ///<tool offset coordinate system
	virtual CoordinateSystem getD(DataPtr data) = 0;///<datas coordinate system	static CoordinateSystem getPr(); ///<patient references coordinate system
	virtual CoordinateSystem getPr() = 0;
	virtual CoordinateSystem getR() = 0; ///<data references coordinate system
	virtual CoordinateSystem convertToSpecific(CoordinateSystem space) = 0; ///< convert "active" references to specific tool/data instances

signals:
	void spaceAddedOrRemoved();
};

} // namespace cx

#endif // CXSPACEPROVIDER_H
