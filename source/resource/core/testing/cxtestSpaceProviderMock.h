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

#ifndef CXTESTSPACEPROVIDERMOCK_H
#define CXTESTSPACEPROVIDERMOCK_H

#include "cxSpaceProvider.h"
#include "cxtestSpaceListenerMock.h"

namespace cxtest
{
typedef boost::shared_ptr<class SpaceProviderMock> SpaceProviderMockPtr;

/**
 *
 * \ingroup cx
 * \date 2014-02-23
 * \author christiana
 */
class SpaceProviderMock : public cx::SpaceProvider
{
public:
	static SpaceProviderMockPtr create() { return SpaceProviderMockPtr(new SpaceProviderMock()); }
	SpaceProviderMock() {}
	virtual ~SpaceProviderMock() {}

	virtual cx::Transform3D get_toMfrom(cx::CoordinateSystem from, cx::CoordinateSystem to) { return cx::Transform3D::Identity(); }
	virtual std::vector<cx::CoordinateSystem> getSpacesToPresentInGUI() { return std::vector<cx::CoordinateSystem>(); }
	virtual std::map<QString, QString> getDisplayNamesForCoordRefObjects() { return std::map<QString, QString>(); }
	virtual cx::SpaceListenerPtr createListener() { return SpaceListenerMock::create(); }

    virtual cx::Vector3D getActiveToolTipPoint(cx::CoordinateSystem to, bool useOffset = false) { return cx::Vector3D::Zero(); }
	virtual cx::Transform3D getActiveToolTipTransform(cx::CoordinateSystem to, bool useOffset = false) { return cx::Transform3D::Identity(); }
	virtual cx::Transform3D get_rMpr() { return cx::Transform3D::Identity(); }

	virtual cx::CoordinateSystem getS(cx::ToolPtr tool) { return cx::CoordinateSystem(cx::csSENSOR); }
	virtual cx::CoordinateSystem getT(cx::ToolPtr tool) { return cx::CoordinateSystem(cx::csTOOL); }
	virtual cx::CoordinateSystem getTO(cx::ToolPtr tool) { return cx::CoordinateSystem(cx::csTOOL_OFFSET); }
	virtual cx::CoordinateSystem getD(cx::DataPtr data) { return cx::CoordinateSystem(cx::csDATA); }
	virtual cx::CoordinateSystem getPr() { return cx::CoordinateSystem(cx::csPATIENTREF); }
	virtual cx::CoordinateSystem getR() { return cx::CoordinateSystem(cx::csREF); }
	virtual cx::CoordinateSystem convertToSpecific(cx::CoordinateSystem space) { return cx::CoordinateSystem(cx::csREF); }
};

} // namespace cxtest

#endif // CXTESTSPACEPROVIDERMOCK_H
