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

#include "cxStringPropertyClipPlane.h"
#include "cxEnumConverter.h"

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