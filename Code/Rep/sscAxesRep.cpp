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

#include "sscAxesRep.h"

#include "boost/bind.hpp"
#include <vtkAxesActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkAssembly.h>

#include "sscTypeConversions.h"
#include "sscView.h"
#include "sscGraphicalPrimitives.h"

namespace ssc
{

AxesRep::AxesRep(const QString& uid) :
	RepImpl(uid)
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&AxesRep::rescale, this));

	mAssembly = vtkAssemblyPtr::New();
	mActor = vtkAxesActorPtr::New();
	mAssembly->AddPart(mActor);
	this->setAxisLength(0.2);

	this->setShowAxesLabels(true);
	setTransform(Transform3D::Identity());
	setFontSize(0.04);

}

void AxesRep::rescale()
{
	if (!mViewportListener->isListening())
		return;
	double size = mViewportListener->getVpnZoom();
	double axisSize = mSize/size;

	mActor->SetTotalLength( axisSize, axisSize, axisSize );
	setTransform(Transform3D(mAssembly->GetUserMatrix()));
}

void AxesRep::setVisible(bool on)
{
	mAssembly->SetVisibility(on);
	for (unsigned i=0; i<mCaption.size(); ++i)
		mCaption[i]->SetVisibility(on);
}

void AxesRep::setShowAxesLabels(bool on)
{
	if (on)
	{
		this->addCaption("x", Vector3D(1,0,0), Vector3D(1,0,0));
		this->addCaption("y", Vector3D(0,1,0), Vector3D(0,1,0));
		this->addCaption("z", Vector3D(0,0,1), Vector3D(0,0,1));
	}
	else
	{
		mCaption.clear();
		mCaptionPos.clear();
	}
}

void AxesRep::setCaption(const QString& caption, const Vector3D& color)
{
	this->addCaption(caption, Vector3D(0,0,0), color);
}

/**set font size to a fraction of the normalized viewport.
 *
 */
void AxesRep::setFontSize(double size)
{
	mFontSize = size;

	for (unsigned i=0; i<mCaption.size(); ++i)
	{
		//mCaption[i]->SetWidth(mFontSize);
		mCaption[i]->SetHeight(mFontSize);
	}
}

/**set axis length to a world length
 *
 */
void AxesRep::setAxisLength(double length)
{
	mSize = length;
	this->rescale();
//	mActor->SetTotalLength( mSize, mSize, mSize );
//	setTransform(Transform3D(mAssembly->GetUserMatrix()));
}

/**Set the position of the axis.
 *
 */
void AxesRep::setTransform(Transform3D rMt)
{
	mAssembly->SetUserMatrix(rMt.getVtkMatrix());

	for (unsigned i=0; i<mCaption.size(); ++i)
	{
		if (!mViewportListener->isListening())
			continue;
		double size = mViewportListener->getVpnZoom();
		double axisSize = mSize/size;

		Vector3D pos = rMt.coord(axisSize*mCaptionPos[i]);
		mCaption[i]->SetAttachmentPoint(pos.begin());
	}
}

void AxesRep::addCaption(const QString& label, Vector3D pos, Vector3D color)
{
	vtkCaptionActor2DPtr cap = vtkCaptionActor2DPtr::New();
	cap->SetCaption(cstring_cast(label));
	cap->GetCaptionTextProperty()->SetColor(color.begin());
	cap->LeaderOff();
	cap->BorderOff();
	cap->GetCaptionTextProperty()->ShadowOff();
	mCaption.push_back(cap);
	mCaptionPos.push_back(pos);
}

AxesRep::~AxesRep()
{
	// ??
}

AxesRepPtr AxesRep::New(const QString& uid)
{
	AxesRepPtr retval(new AxesRep(uid));
	retval->mSelf = retval;
	return retval;
}

void AxesRep::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddActor(mAssembly);
	for (unsigned i=0; i<mCaption.size(); ++i)
		view->getRenderer()->AddActor(mCaption[i]);
	mViewportListener->startListen(view->getRenderer());
	this->rescale();
}

void AxesRep::removeRepActorsFromViewRenderer(View *view)
{
	view->getRenderer()->RemoveActor(mAssembly);
	for (unsigned i=0; i<mCaption.size(); ++i)
	{
		view->getRenderer()->RemoveActor(mCaption[i]);
	}
	mViewportListener->stopListen();
}

} // namespace ssc
