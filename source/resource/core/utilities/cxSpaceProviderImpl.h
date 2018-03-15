/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
