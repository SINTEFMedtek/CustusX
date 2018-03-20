/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStringPropertyClipPlane.h"
#include "cxEnumConverter.h"
#include "cxDefinitionStrings.h"

namespace cx
{

StringPropertyClipPlane::StringPropertyClipPlane(InteractiveClipperPtr clipper) :
	mInteractiveClipper(clipper)
{
	connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}

QString StringPropertyClipPlane::getDisplayName() const
{
	return "Slice Plane";
}
bool StringPropertyClipPlane::setValue(const QString& value)
{
	PLANE_TYPE plane = string2enum<PLANE_TYPE> (value);
	if (plane == mInteractiveClipper->getSlicePlane())
		return false;
	mInteractiveClipper->setSlicePlane(plane);
	return true;
}
QString StringPropertyClipPlane::getValue() const
{
	return qstring_cast(mInteractiveClipper->getSlicePlane());
}
QString StringPropertyClipPlane::getHelp() const
{
	return "Chose the slice plane to clip with.";
}
QStringList StringPropertyClipPlane::getValueRange() const
{
	std::vector<PLANE_TYPE> planes = mInteractiveClipper->getAvailableSlicePlanes();
	QStringList retval;
	for (unsigned i = 0; i < planes.size(); ++i)
		retval << qstring_cast(planes[i]);
	return retval;
}

void StringPropertyClipPlane::setClipper(InteractiveClipperPtr clipper)
{
	mInteractiveClipper = clipper;
}

}//cx