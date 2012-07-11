// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * vmCrossHairRep2D.cpp
 *
 *  Created on: Jan 13, 2009
 *      Author: christiana
 */

#include "sscCrossHairRep2D.h"

#include <vtkActor2D.h>
#include <vtkRenderer.h>

#include "sscView.h"
#include "sscToolManager.h"
#include "sscSliceProxy.h"
#include "sscVtkHelperClasses.h"

namespace ssc
{

CrossHairRep2D::CrossHairRep2D(const QString& uid, const QString& name) :
	RepImpl(uid, name),
	m_vpMs(Transform3D::Identity())
{
}

CrossHairRep2D::~CrossHairRep2D()
{
}

CrossHairRep2DPtr CrossHairRep2D::New(const QString& uid, const QString& name)
{
	CrossHairRep2DPtr retval(new CrossHairRep2D(uid, name));
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

void CrossHairRep2D::setSliceProxy(ssc::SliceProxyPtr slicer)
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

void CrossHairRep2D::addRepActorsToViewRenderer(ssc::View *view)
{
	mCursor.reset( new ssc::CrossHair2D(view->getRenderer()) ) ;
	double bordarOffset = 150.0;
	ssc::RGBColor color( 1.0, 0.8, 0.0 );
	mCursor->setValue( Vector3D(0,0,0), 100, 100, bordarOffset, color );
	update();
}

void CrossHairRep2D::removeRepActorsFromViewRenderer(ssc::View *view)
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
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D sMr = mSlicer->get_sMr();

	if (mCursor)
	{
		Transform3D vpMt = m_vpMs*sMr*rMpr*prMt;
		mCursor->updateRegCross( vpMt.coord(Vector3D(0,0,0)) );
		//Logger::log("vm.log", "CrossHairRep2D::update(), center=" + string_cast(vpMt.coord(Vector3D(0,0,0))));
	}
}

}
