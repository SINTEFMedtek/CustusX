/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
