#include "sscVolumetricRep.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include "sscView.h"

namespace ssc
{

VolumetricRep::VolumetricRep(const std::string& uid) :
	RepImpl(uid),
	mOpacityTransferFunction(vtkPiecewiseFunctionPtr::New()),
	mColorTransferFunction(vtkColorTransferFunctionPtr::New()),
	mVolumeProperty(vtkVolumePropertyPtr::New())
{
	mOpacityTransferFunction->AddPoint(0.0, 0.0);
	mOpacityTransferFunction->AddPoint(500, 1.0);

	mColorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTransferFunction->AddRGBPoint(500.0, 1.0, 1.0, 1.0);

	mVolumeProperty->SetColor(mColorTransferFunction);
	mVolumeProperty->SetScalarOpacity(mOpacityTransferFunction);
	mVolumeProperty->SetInterpolationTypeToLinear();

	mTextureMapper3D = vtkVolumeTextureMapper3DPtr::New();

	mVolume = vtkVolumePtr::New();
	mVolume->SetProperty( mVolumeProperty );
	mVolume->SetMapper( mTextureMapper3D );
}

VolumetricRep::~VolumetricRep()
{
	// ??
}

VolumetricRepPtr VolumetricRep::create(const std::string& uid)
{
	VolumetricRepPtr retval(new VolumetricRep(uid));
	retval->mSelf = retval;
	return retval;
}

void VolumetricRep::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddVolume(mVolume);
}

void VolumetricRep::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveVolume(mVolume);
}
void VolumetricRep::setImage(ImagePtr image)
{
	mImage = image;
	mImage->connectRep(mSelf);

	mTextureMapper3D->SetInput( image->getRefVtkImageData() );
}
//void VolumeRep::addImage(ImagePtr image)
//{}
/*void VolumeRep::removeImage(ImagePtr image)
{
	mImage->disconnectRep(mSelf);
	mTextureMapper3D->SetInput( (vtkImageData*)NULL );

	mImage.reset();
}*/
bool VolumetricRep::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}
} // namespace ssc
