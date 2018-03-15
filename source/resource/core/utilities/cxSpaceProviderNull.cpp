/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSpaceProviderNull.h"


namespace cx
{

SpaceProviderNull::SpaceProviderNull()
{

}

Transform3D SpaceProviderNull::get_toMfrom(CoordinateSystem from, CoordinateSystem to)
{
	return Transform3D::Identity();
}

std::vector<CoordinateSystem> SpaceProviderNull::getSpacesToPresentInGUI()
{
	return std::vector<CoordinateSystem>();
}

std::map<QString, QString> SpaceProviderNull::getDisplayNamesForCoordRefObjects()
{
	return std::map<QString, QString>();
}

SpaceListenerPtr SpaceProviderNull::createListener()
{
	return SpaceListenerPtr();
}

Vector3D SpaceProviderNull::getActiveToolTipPoint(CoordinateSystem to, bool useOffset)
{
	return Vector3D();
}

Transform3D SpaceProviderNull::getActiveToolTipTransform(CoordinateSystem to, bool useOffset)
{
	return Transform3D::Identity();
}

Transform3D SpaceProviderNull::get_rMpr()
{
	return Transform3D::Identity();
}

CoordinateSystem SpaceProviderNull::getS(ToolPtr tool)
{
	return CoordinateSystem();
}

CoordinateSystem SpaceProviderNull::getT(ToolPtr tool)
{
	return CoordinateSystem();
}

CoordinateSystem SpaceProviderNull::getTO(ToolPtr tool)
{
	return CoordinateSystem();
}

CoordinateSystem SpaceProviderNull::getD(DataPtr data)
{
	return CoordinateSystem();
}

CoordinateSystem SpaceProviderNull::getPr()
{
	return CoordinateSystem();
}

CoordinateSystem SpaceProviderNull::getR()
{
	return CoordinateSystem();
}

CoordinateSystem SpaceProviderNull::convertToSpecific(CoordinateSystem space)
{
	return CoordinateSystem();
}

} // namespace cx
