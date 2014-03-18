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
#ifndef CXSPACEPROVIDER_H
#define CXSPACEPROVIDER_H

#include "cxTransform3D.h"
#include "cxCoordinateSystemHelpers.h"

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
class SpaceProvider
{
public:
	virtual ~SpaceProvider() {}

	virtual Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to) = 0; ///< to_M_from
	virtual std::vector<CoordinateSystem> getSpacesToPresentInGUI() = 0;
	virtual SpaceListenerPtr createListener() = 0;

	virtual Vector3D getDominantToolTipPoint(CoordinateSystem to, bool useOffset = false) = 0; ///< P_to, dominant tools current point in coord
	virtual Transform3D getDominantToolTipTransform(CoordinateSystem to, bool useOffset = false) = 0;///< Get toMt, where t is dominant tool
	virtual Transform3D get_rMpr() = 0;

	virtual CoordinateSystem getS(ToolPtr tool) = 0; ///<tools sensor coordinate system
	virtual CoordinateSystem getT(ToolPtr tool) = 0; ///<tools coordinate system
	virtual CoordinateSystem getTO(ToolPtr tool) = 0; ///<tool offset coordinate system
	virtual CoordinateSystem getD(DataPtr data) = 0;///<datas coordinate system	static CoordinateSystem getPr(); ///<patient references coordinate system
	virtual CoordinateSystem getPr() = 0;
	virtual CoordinateSystem getR() = 0; ///<data references coordinate system
};

} // namespace cx

#endif // CXSPACEPROVIDER_H
