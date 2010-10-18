#include "sscSliceRep.h"

#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "sscView.h"
#include "sscImage.h"

namespace ssc
{
SliceRep::SliceRep(const QString& uid) :
	RepImpl(uid)
{
	mVtkImagePlaneWidget = vtkImagePlaneWidgetPtr::New();
	mVtkImagePlaneWidget->SetPlaneOrientationToZAxes();//Convenience method sets the plane orientation normal to the x, y, or z axes
}

SliceRep::~SliceRep()
{
}

SliceRepPtr SliceRep::New(const QString& uid)
{
	SliceRepPtr retval(new SliceRep(uid));
	retval->mSelf = retval;
	return retval;
}

void SliceRep::addRepActorsToViewRenderer(View* view)
{
	mVtkImagePlaneWidget->SetInteractor( view->getRenderWindow()->GetInteractor() );
	mVtkImagePlaneWidget->InteractionOn();
	mVtkImagePlaneWidget->On();
}

void SliceRep::removeRepActorsFromViewRenderer(View* view)
{
	
}
void SliceRep::setImage(ImagePtr image)
{
	mImage = image;
	mImage->connectToRep(mSelf);
	mVtkImagePlaneWidget->SetInput( image->getBaseVtkImageData() );
}

bool SliceRep::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}

	//void SliceRep::setSliceProxy(SliceProxyPtr proxy) //connect tool transform update to sliceRep
	//{
	//	
	//}

} // namespace ssc
