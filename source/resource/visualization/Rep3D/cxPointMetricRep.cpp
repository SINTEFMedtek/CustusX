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



#include "cxPointMetricRep.h"
#include "cxView.h"
#include "boost/bind.hpp"
#include "cxLogger.h"

namespace cx
{

PointMetricRepPtr PointMetricRep::New(const QString& uid, const QString& name)
{
	PointMetricRepPtr retval(new PointMetricRep(uid, name));
	return retval;
}

PointMetricRep::PointMetricRep(const QString& uid, const QString& name) :
                DataMetricRep(uid, name)
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&PointMetricRep::rescale, this));
}

void PointMetricRep::clear()
{
    DataMetricRep::clear();
    mGraphicalPoint.reset();
}

void PointMetricRep::addRepActorsToViewRenderer(View *view)
{
    mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void PointMetricRep::removeRepActorsFromViewRenderer(View *view)
{
    DataMetricRep::removeRepActorsFromViewRenderer(view);
    mViewportListener->stopListen();
}

void PointMetricRep::onModifiedStartRender()
{
    if (!mMetric)
		return;

	if (!mGraphicalPoint && mView && mMetric)
		mGraphicalPoint.reset(new GraphicalPoint3D(mView->getRenderer()));

	if (!mGraphicalPoint)
		return;

    Vector3D p0_r = mMetric->getRefCoord();

	mGraphicalPoint->setValue(p0_r);
	mGraphicalPoint->setRadius(mGraphicsSize);
	mGraphicalPoint->setColor(mMetric->getColor());

    this->drawText();

    this->rescale();
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void PointMetricRep::rescale()
{
	if (!mGraphicalPoint)
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;
	mGraphicalPoint->setRadius(sphereSize);
}

}
