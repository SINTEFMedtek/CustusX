#include "sscSlicePlaneRep.h"
#include <vtkImageActor.h>
#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscDataManager.h"

namespace ssc
{

SlicePlaneRep::SlicePlaneRep(const std::string& uid) :
	RepImpl(uid)
{
	mVtkImagePlaneWidgetA = vtkImagePlaneWidgetPtr::New();
	mVtkImagePlaneWidgetA->SetPlaneOrientationToZAxes();//Convenience method sets the plane orientation normal to the x, y, or z axes
}

SlicePlaneRep::~SlicePlaneRep()
{
}

SlicePlaneRepPtr SlicePlaneRep::New(const std::string& uid)
{
	SlicePlaneRepPtr retval(new SlicePlaneRep(uid));
	retval->mSelf = retval;
	return retval;
}

void SlicePlaneRep::addRepActorsToViewRenderer(View* view)
{
	mVtkImagePlaneWidgetA->SetInteractor( view->getRenderWindow()->GetInteractor() );
	mVtkImagePlaneWidgetA->InteractionOn();
	mVtkImagePlaneWidgetA->On();
//	
//	for(unsigned i=0; i<mSlicesPlanes.size(); ++i)
//	{
//		view->getRenderer()->AddActor( mSlicesPlanes.at(i)->getActor() );
//	}
}

void SlicePlaneRep::removeRepActorsFromViewRenderer(View* view)
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
	
	Vector3D c = ssc::DataManager::getInstance()->getCenter();

	
}
void SlicePlaneRep::sliceTransformChangedSlot(Transform3D sMr)
{
	
}

} // namespace ssc
