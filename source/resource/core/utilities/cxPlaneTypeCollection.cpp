/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


