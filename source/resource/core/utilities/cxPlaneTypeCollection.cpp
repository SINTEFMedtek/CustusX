/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxPlaneTypeCollection.h"

#include "cxDefinitionStrings.h"
#include <iostream>

namespace cx
{

PlaneTypeCollection::PlaneTypeCollection(std::vector<PLANE_TYPE> planes) :
	mPlanes(planes)
{
}

std::vector<PLANE_TYPE> PlaneTypeCollection::get() const
{
	return mPlanes;
}

void PlaneTypeCollection::add(PLANE_TYPE plane)
{
	if (plane!=ptCOUNT)
		mPlanes.push_back(plane);
}

PlaneTypeCollection::PlaneTypeCollection(PLANE_TYPE a, PLANE_TYPE b, PLANE_TYPE c)
{
	this->add(a);
	this->add(b);
	this->add(c);
}

PlaneTypeCollection PlaneTypeCollection::fromString(QString input, PlaneTypeCollection defVal)
{
	std::vector<PLANE_TYPE> planes;
	QStringList planeText = input.split("/");
	for (unsigned i=0; i<planeText.size(); ++i)
	{
		PLANE_TYPE plane = string2enum<PLANE_TYPE>(planeText[i]);
		if (plane!=ptCOUNT)
			planes.push_back(plane);
	}

	if (planes.empty())
		return defVal;

	return PlaneTypeCollection(planes);
}

QString PlaneTypeCollection::toString() const
{
	QStringList planeText;
	for (unsigned i=0; i<mPlanes.size(); ++i)
		planeText << enum2string(mPlanes[i]);
	QString retval = planeText.join("/");
	return retval;
}


} // namespace cx


