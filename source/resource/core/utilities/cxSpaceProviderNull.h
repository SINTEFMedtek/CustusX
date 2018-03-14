/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef SPACEPROVIDERNULL_H
#define SPACEPROVIDERNULL_H

#include "cxSpaceProvider.h"

namespace cx
{

class cxResource_EXPORT SpaceProviderNull : public SpaceProvider
{
public:
	SpaceProviderNull();
	Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to);
	std::vector<CoordinateSystem> getSpacesToPresentInGUI();
	std::map<QString, QString> getDisplayNamesForCoordRefObjects();
	SpaceListenerPtr createListener();
	Vector3D getActiveToolTipPoint(CoordinateSystem to, bool useOffset);
	Transform3D getActiveToolTipTransform(CoordinateSystem to, bool useOffset);
	Transform3D get_rMpr();
	CoordinateSystem getS(ToolPtr tool);
	CoordinateSystem getT(ToolPtr tool);
	CoordinateSystem getTO(ToolPtr tool);
	CoordinateSystem getD(DataPtr data);
	CoordinateSystem getPr();
	CoordinateSystem getR();
	CoordinateSystem convertToSpecific(CoordinateSystem space);
};

} // namespace cx

#endif // SPACEPROVIDERNULL_H
