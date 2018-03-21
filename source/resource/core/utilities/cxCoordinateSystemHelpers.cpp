/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCoordinateSystemHelpers.h"

#include "cxDefinitionStrings.h"

namespace cx
{

QString CoordinateSystem::toString() const
{
	return enum2string(mId) + (mRefObject.isEmpty() ? "" : ("/"+mRefObject));
}

CoordinateSystem CoordinateSystem::fromString(QString text)
{
	QStringList raw = text.split("/");
	CoordinateSystem retval(csCOUNT);
	if (raw.size()<1)
		return retval;
	retval.mId = string2enum<COORDINATE_SYSTEM>(raw[0]);
	if (raw.size()<2)
		return retval;
	retval.mRefObject = raw[1];
	return retval;
}

bool CoordinateSystem::isValid() const
{
	return mId!=csCOUNT;
}

bool operator==(const CoordinateSystem& lhs, const CoordinateSystem& rhs)
{
	return ( lhs.mId==rhs.mId )&&( lhs.mRefObject==rhs.mRefObject );
}

// --------------------------------------------------------



} //namespace cx
