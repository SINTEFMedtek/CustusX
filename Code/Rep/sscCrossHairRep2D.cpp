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

CrossHairRep2D::CrossHairRep2D(const std::string& uid, const std::string& name) :
	RepImpl(uid, name)
{
}

CrossHairRep2D::~CrossHairRep2D()
{
}

CrossHairRep2DPtr CrossHairRep2D::New(const std::string& uid, const std::string& name)
{
	CrossHairRep2DPtr retval(new CrossHairRep2D(uid, name));
	retval->mSelf = retval;
	return retval;
}

std::string CrossHairRep2D::getType() const
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

void CrossHairRep2D::addRepActorsToViewRenderer(ssc::View* view)
{
	mCursor.reset( new ssc::CrossHair2D(view->getRenderer()) ) ;
	double bordarOffset = 150.0;
	ssc::RGBColor color( 1.0, 0.8, 0.0 );
	mCursor->setValue( Vector3D(0,0,0), 100, 100, bordarOffset, color );
	update();
}

void CrossHairRep2D::removeRepActorsFromViewRenderer(ssc::View* view)
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
