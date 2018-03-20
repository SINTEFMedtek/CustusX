/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxSpaceProvider.h"

namespace cx
{

CrossHairRep2D::CrossHairRep2D(SpaceProviderPtr spaceProvider) :
	RepImpl(),
	m_vpMs(Transform3D::Identity()),
	mSpaceProvider(spaceProvider)
{
}

CrossHairRep2D::~CrossHairRep2D()
{
}

CrossHairRep2DPtr CrossHairRep2D::New(SpaceProviderPtr spaceProvider, const QString& uid)
{
	return wrap_new(new CrossHairRep2D(spaceProvider), uid);
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

void CrossHairRep2D::addRepActorsToViewRenderer(ViewPtr view)
{
	mCursor.reset( new CrossHair2D(view->getRenderer()) ) ;
	double bordarOffset = 150.0;
	RGBColor color( 1.0, 0.8, 0.0 );
	mCursor->setValue( Vector3D(0,0,0), 100, 100, bordarOffset, color );
	update();
}

void CrossHairRep2D::removeRepActorsFromViewRenderer(ViewPtr view)
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
