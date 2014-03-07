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
#ifndef CXSPACEPROVIDERIMPL_H
#define CXSPACEPROVIDERIMPL_H

#include "cxSpaceProvider.h"
#include "cxForwardDeclarations.h"

namespace cx
{
class ToolManager;
class DataManager;

/** Provides information about all the coordinate systems in the application.
 *
 *
 * \ingroup cx
 * \date 2014-02-21
 * \author christiana
 */
class SpaceProviderImpl : public SpaceProvider
{
public:
	SpaceProviderImpl(TrackingServicePtr toolManager, DataServicePtr dataManager);
	virtual ~SpaceProviderImpl() {}

	virtual Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to); ///< to_M_from
	virtual std::vector<CoordinateSystem> getSpacesToPresentInGUI();
	virtual SpaceListenerPtr createListener();

	virtual Vector3D getDominantToolTipPoint(CoordinateSystem to, bool useOffset = false); ///< P_to, dominant tools current point in coord
	virtual Transform3D getDominantToolTipTransform(CoordinateSystem to, bool useOffset = false);///< Get toMt, where t is dominant tool
	virtual Transform3D get_rMpr();

	virtual CoordinateSystem getS(ToolPtr tool); ///<tools sensor coordinate system
	virtual CoordinateSystem getT(ToolPtr tool); ///<tools coordinate system
	virtual CoordinateSystem getTO(ToolPtr tool); ///<tool offset coordinate system
	virtual CoordinateSystem getD(DataPtr data);///<datas coordinate system	static CoordinateSystem getPr(); ///<patient references coordinate system
	virtual CoordinateSystem getPr();
	virtual CoordinateSystem getR(); ///<data references coordinate system

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

	TrackingServicePtr mToolManager;
	DataServicePtr mDataManager;
};

} // namespace cx

#endif // CXSPACEPROVIDERIMPL_H
