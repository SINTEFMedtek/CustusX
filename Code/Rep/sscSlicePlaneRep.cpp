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

#include "sscSlicePlaneRep.h"

#include <vtkImageActor.h>
#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "sscSlicerRepSW.h"
#include "sscSliceProxy.h"
#include "sscView.h"
#include "sscImage.h"
#include "sscDataManager.h"

namespace ssc
{

SlicePlaneRep::SlicePlaneRep(const QString& uid) :
	RepImpl(uid)
{
	mVtkImagePlaneWidgetA = vtkImagePlaneWidgetPtr::New();
	mVtkImagePlaneWidgetA->SetPlaneOrientationToZAxes();//Convenience method sets the plane orientation normal to the x, y, or z axes
}

SlicePlaneRep::~SlicePlaneRep()
{
}

SlicePlaneRepPtr SlicePlaneRep::New(const QString& uid)
{
	SlicePlaneRepPtr retval(new SlicePlaneRep(uid));
	retval->mSelf = retval;
	return retval;
}

void SlicePlaneRep::addRepActorsToViewRenderer(View *view)
{
	mVtkImagePlaneWidgetA->SetInteractor( view->getRenderWindow()->GetInteractor() );
	mVtkImagePlaneWidgetA->InteractionOn();
	mVtkImagePlaneWidgetA->On();
}

void SlicePlaneRep::removeRepActorsFromViewRenderer(View *view)
{
	for(unsigned i=0; i<mSlicesPlanes.size(); ++i)
	{
		view->getRenderer()->RemoveActor( mSlicesPlanes.at(i)->getActor() );
	}
}

void SlicePlaneRep::setImage(ImagePtr image)
{
	mImage = image;
	mImage->connectToRep(mSelf);
	mVtkImagePlaneWidgetA->SetInput( image->getBaseVtkImageData() );
}

bool SlicePlaneRep::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}

void SlicePlaneRep::setSliceReps( std::vector<ssc::SliceRepSWPtr> slicerReps)
{
	mSlicesPlanes = slicerReps;
	setImage(mSlicesPlanes.at(0)->getImage() );
//	for(unsigned i=0; i<mSlicesPlanes.size(); ++i)
//	{
//		mImageActors.push_back( slicerReps.at(i)->getActor() );
//	}

}

void SlicePlaneRep::setSliceProxy(SliceProxyPtr proxy) //connect tool transform update to sliceRep
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	}
	mSlicer = proxy;
	connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
}

void SlicePlaneRep::sliceTransformChangedSlot(Transform3D sMr)
{
}

} // namespace ssc
