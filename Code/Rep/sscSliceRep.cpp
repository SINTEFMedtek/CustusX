#include "sscSliceRep.h"

#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "sscView.h"

namespace ssc
{

SliceRep::SliceRep(const std::string& uid) :
	RepImpl(uid)
{

	mVtkImagePlaneWidget = vtkImagePlaneWidgetPtr::New();
	mVtkImagePlaneWidget->SetPlaneOrientationToZAxes();

}

SliceRep::~SliceRep()
{
}

SliceRepPtr SliceRep::New(const std::string& uid)
{
	SliceRepPtr retval(new SliceRep(uid));
	retval->mSelf = retval;
	return retval;
}

void SliceRep::addRepActorsToViewRenderer(View* view)
{
	//view->getRenderer()->AddVolume(mVolume);
	mVtkImagePlaneWidget->SetInteractor( view->getRenderWindow()->GetInteractor() );
	mVtkImagePlaneWidget->InteractionOn();
	mVtkImagePlaneWidget->On();
}

void SliceRep::removeRepActorsFromViewRenderer(View* view)
{
	//view->getRenderer()->RemoveVolume(mVolume);
}
void SliceRep::setImage(ImagePtr image)
{
	mImage = image;
	mImage->connectRep(mSelf);

	//mTextureMapper3D->SetInput( image->getRefVtkImageData() );
	mVtkImagePlaneWidget->SetInput( image->getBaseVtkImageData() );
}

bool SliceRep::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}

} // namespace ssc
