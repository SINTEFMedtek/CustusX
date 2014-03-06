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
#include "cxPlaneTypeCollection.h"

#include "sscDefinitionStrings.h"
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


