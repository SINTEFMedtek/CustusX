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


#include "cxDistanceMetricRep.h"
#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "cxTypeConversions.h"

namespace cx
{

DistanceMetricRepPtr DistanceMetricRep::New(const QString& uid)
{
	return wrap_new(new DistanceMetricRep(), uid);
}

DistanceMetricRep::DistanceMetricRep()
{
}

void DistanceMetricRep::clear()
{
    DataMetricRep::clear();
    mGraphicalLine.reset();
}

DistanceMetricPtr DistanceMetricRep::getDistanceMetric()
{
    return boost::dynamic_pointer_cast<DistanceMetric>(mMetric);
}

void DistanceMetricRep::onModifiedStartRender()
{
	DistanceMetricPtr distanceMetric = this->getDistanceMetric();
	if (!distanceMetric)
		return;

	if (!mMetric->isValid())
		return;

	if (!mGraphicalLine && this->getView())
	{
		mGraphicalLine.reset(new GraphicalLine3D(this->getRenderer()));
	}

	if (mGraphicalLine)
	{
		mGraphicalLine->setColor(mMetric->getColor());
		std::vector<Vector3D> p = distanceMetric->getEndpoints();
		mGraphicalLine->setValue(p[0], p[1]);
		mGraphicalLine->setStipple(0xF0FF);
	}

	this->drawText();
}


}
