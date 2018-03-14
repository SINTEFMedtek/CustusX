/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	static SpaceProviderPtr getNullObject();

signals:
	void spaceAddedOrRemoved();
};

} // namespace cx

#endif // CXSPACEPROVIDER_H
