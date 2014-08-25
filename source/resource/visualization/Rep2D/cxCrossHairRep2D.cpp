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


/*
 * vmCrossHairRep2D.cpp
 *
 *  Created on: Jan 13, 2009
 *      Author: christiana
 */

#include "cxCrossHairRep2D.h"

#include <vtkActor2D.h>
#include <vtkRenderer.h>

#include "cxTool.h"
#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxVtkHelperClasses.h"
#include "cxDataManager.h"
#include "cxSpaceProvider.h"

namespace cx
{

CrossHairRep2D::CrossHairRep2D(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name) :
	RepImpl(uid, name),
	m_vpMs(Transform3D::Identity()),
	mSpaceProvider(spaceProvider)
{
}

CrossHairRep2D::~CrossHairRep2D()
{
}

CrossHairRep2DPtr CrossHairRep2D::New(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name)
{
	CrossHairRep2DPtr retval(new CrossHairRep2D(spaceProvider, uid, name));
	retval->mSelf = retval;
	return retval;
}

QString CrossHairRep2D::getType() const
{
	return "vm::CrossHairRep2D";
}

void CrossHairRep2D::set_vpMs(const Transform3D& vpMs)
{
	m_vpMs = vpMs;
	update();
}

void CrossHairRep2D::setSliceProxy(SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
		disconnect(mSlicer.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		disconnect(mSlicer.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
	}

	mSlicer = slicer;

	connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	connect(mSlicer.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
	connect(mSlicer.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));

	update();
}

void CrossHairRep2D::addRepActorsToViewRenderer(View *view)
{
	mCursor.reset( new CrossHair2D(view->getRenderer()) ) ;
	double bordarOffset = 150.0;
	RGBColor color( 1.0, 0.8, 0.0 );
	mCursor->setValue( Vector3D(0,0,0), 100, 100, bordarOffset, color );
	update();
}

void CrossHairRep2D::removeRepActorsFromViewRenderer(View *view)
{
	mCursor.reset();
}

void CrossHairRep2D::sliceTransformChangedSlot(Transform3D sMr)
{
	update();
}
void CrossHairRep2D::toolTransformAndTimestampSlot(Transform3D prMt, double timestamp)
{
	update();
}
void CrossHairRep2D::toolVisibleSlot(bool visible)
{
	update();
}

void CrossHairRep2D::update()
{
	if (!mSlicer || !mSlicer->getTool())
		return;

	Transform3D prMt = mSlicer->getTool()->get_prMt();
	Transform3D rMpr = mSpaceProvider->get_rMpr();
	Transform3D sMr = mSlicer->get_sMr();

	if (mCursor)
	{
		Transform3D vpMt = m_vpMs*sMr*rMpr*prMt;
		mCursor->updateRegCross( vpMt.coord(Vector3D(0,0,0)) );
		//Logger::log("vm.log", "CrossHairRep2D::update(), center=" + string_cast(vpMt.coord(Vector3D(0,0,0))));
	}
}

}
