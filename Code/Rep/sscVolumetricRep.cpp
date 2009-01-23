#include "sscVolumetricRep.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>

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

	// from snws
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

ImagePtr VolumetricRep::getImage()
{
	return mImage;
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
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		//disconnect(this, SIGNAL(addPermanentPoint(double, double, double)),
		//			mImage.get(), SLOT(addLandmarkSlot(double, double, double)));
	}

	mImage = image;

	if (mImage)
	{
		mImage->connectRep(mSelf);
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		//connect(this, SIGNAL(addPermanentPoint(double, double, double)),
		//			mImage.get(), SLOT(addLandmarkSlot(double, double, double)));
		vtkImageDataChangedSlot();
	}
	else
	{
		mTextureMapper3D->SetInput( (vtkImageData*)NULL );
	}
}

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
	mVolumeProperty->SetColor(mImage->transferFunctions3D().getColorTF());
	mVolumeProperty->SetScalarOpacity(mImage->transferFunctions3D().getOpacityTF());
	// use the base instead of the ref image, because otherwise changes in the transform 
	// causes data to be sent anew to the graphics card (takes 4s).
	// changing the mVolume transform instead is a fast operation.
	mTextureMapper3D->SetInput( mImage->getBaseVtkImageData() );
	transformChangedSlot();
}

/**called when transform is changed
 * reset it in the prop.
 */
void VolumetricRep::transformChangedSlot()
{
	if (!mImage)
	{
		return;
	}
	//std::cout << "VolumetricRep::transformChangedSlot() \n" << mImage->getTransform() << std::endl;
	mVolume->SetUserMatrix(mImage->getTransform().inv().matrix());
}


} // namespace ssc
