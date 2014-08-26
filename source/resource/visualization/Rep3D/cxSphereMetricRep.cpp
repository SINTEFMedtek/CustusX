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

#include "cxSphereMetricRep.h"

#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "cxTypeConversions.h"
#include "vtkTextActor.h"
#include "cxGraphicalPrimitives.h"
#include "cxSphereMetric.h"
#include "cxGraphicalPrimitives.h"

namespace cx
{

SphereMetricRepPtr SphereMetricRep::New(const QString& uid, const QString& name)
{
	SphereMetricRepPtr retval(new SphereMetricRep(uid, name));
	return retval;
}

SphereMetricRep::SphereMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name)
{
}

void SphereMetricRep::clear()
{
	DataMetricRep::clear();
	mGraphicalPoint.reset();
}

SphereMetricPtr SphereMetricRep::getSphereMetric()
{
	return boost::dynamic_pointer_cast<SphereMetric>(mMetric);
}

void SphereMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

	if (!mGraphicalPoint && mView && mMetric)
		mGraphicalPoint.reset(new GraphicalPoint3D(mView->getRenderer()));

	if (!mGraphicalPoint)
		return;

	mGraphicalPoint->setValue(mMetric->getRefCoord());
	mGraphicalPoint->setRadius(this->getSphereMetric()->getRadius());
	mGraphicalPoint->setColor(mMetric->getColor());

	this->drawText();
}

}
