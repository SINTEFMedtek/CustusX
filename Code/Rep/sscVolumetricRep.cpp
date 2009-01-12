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

VolumetricRep::VolumetricRep(const std::string& uid, const std::string& name) :
	RepImpl(uid, name),
	mOpacityTransferFunction(vtkPiecewiseFunctionPtr::New()),
	mColorTransferFunction(vtkColorTransferFunctionPtr::New()),
	mVolumeProperty(vtkVolumePropertyPtr::New()),
	mTextureMapper3D(vtkVolumeTextureMapper3DPtr::New()),
	mVolume(vtkVolumePtr::New())
{
	double maxVal = 255;//500.0;

	mOpacityTransferFunction->AddPoint(0.0, 0.0);
	mOpacityTransferFunction->AddPoint(maxVal, 1.0);

	mColorTransferFunction->SetColorSpaceToRGB();
	mColorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTransferFunction->AddRGBPoint(maxVal, 1.0, 1.0, 1.0);

	mVolumeProperty->SetColor(mColorTransferFunction);
	mVolumeProperty->SetScalarOpacity(mOpacityTransferFunction);
	mVolumeProperty->SetInterpolationTypeToLinear();

	// from snw
	mVolumeProperty->ShadeOff();
	mVolumeProperty->SetAmbient ( 0.2 );
	mVolumeProperty->SetDiffuse ( 0.9 );
	mVolumeProperty->SetSpecular ( 0.3 );
	mVolumeProperty->SetSpecularPower ( 15.0 );
	mVolumeProperty->SetScalarOpacityUnitDistance(0.8919);

	// from snw
	mTextureMapper3D->SetPreferredMethodToNVidia();
    mTextureMapper3D->SetBlendModeToComposite();

	mVolume->SetProperty( mVolumeProperty );
	mVolume->SetMapper( mTextureMapper3D );
}



VolumetricRep::~VolumetricRep()
{
	// ??
}

VolumetricRepPtr VolumetricRep::New(const std::string& uid, const std::string& name)
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
	if (image==mImage)
	{
		return;
	}

	if (mImage)
	{

		mImage->disconnectRep(mSelf);
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
	}

	mImage = image;

	if (mImage)
	{
		mImage->connectRep(mSelf);
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		//mLUT = mImage->getLut();
		mVolumeProperty->SetColor(mImage->transferFunctions3D().getColorTF());
		mVolumeProperty->SetScalarOpacity(mImage->transferFunctions3D().getOpacityTF());
		mTextureMapper3D->SetInput( mImage->getRefVtkImageData() );
	}
	else
	{
		mTextureMapper3D->SetInput( (vtkImageData*)NULL );
	}
}
//void VolumeRep::addImage(ImagePtr image)
//{}
/*void VolumeRep::removeImage(ImagePtr image)
{
v	mImage->disconnectRep(mSelf);
	mTextureMapper3D->SetInput( (vtkImageData*)NULL );

	mImage.reset();
}*/
bool VolumetricRep::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}

/**called when the image is changed internally.
 * re-read the lut and vtkimagedata.
 */
void VolumetricRep::vtkImageDataChangedSlot()
{
	if (!mImage)
	{
		return;
	}

	//mLUT = mImage->getLut();
	mVolumeProperty->SetColor(mImage->transferFunctions3D().getColorTF());
	mVolumeProperty->SetScalarOpacity(mImage->transferFunctions3D().getOpacityTF());
	mTextureMapper3D->SetInput( mImage->getRefVtkImageData() );
	// TODO CA test if we need more update of the texturemapper...
}


} // namespace ssc
