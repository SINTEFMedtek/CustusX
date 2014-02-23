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
	virtual cx::SpaceListenerPtr createListener() { return SpaceListenerMock::create(); }

	virtual cx::Vector3D getDominantToolTipPoint(cx::CoordinateSystem to, bool useOffset = false) { return cx::Vector3D::Zero(); }
	virtual cx::Transform3D getDominantToolTipTransform(cx::CoordinateSystem to, bool useOffset = false) { return cx::Transform3D::Identity(); }

	virtual cx::CoordinateSystem getS(cx::ToolPtr tool) { return cx::CoordinateSystem(cx::csSENSOR); }
	virtual cx::CoordinateSystem getT(cx::ToolPtr tool) { return cx::CoordinateSystem(cx::csTOOL); }
	virtual cx::CoordinateSystem getTO(cx::ToolPtr tool) { return cx::CoordinateSystem(cx::csTOOL_OFFSET); }
	virtual cx::CoordinateSystem getD(cx::DataPtr data) { return cx::CoordinateSystem(cx::csDATA); }
	virtual cx::CoordinateSystem getPr() { return cx::CoordinateSystem(cx::csPATIENTREF); }
	virtual cx::CoordinateSystem getR() { return cx::CoordinateSystem(cx::csREF); }
};

} // namespace cxtest

#endif // CXTESTSPACEPROVIDERMOCK_H
