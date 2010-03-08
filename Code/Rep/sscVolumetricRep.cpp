#include "sscVolumetricRep.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkImageResample.h>

#include "sscView.h"
#include "sscImageTF3D.h"


typedef vtkSmartPointer<class vtkImageResample> vtkImageResamplePtr;

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
	mResampleFactor = 1.0;
	//double maxVal = 255;//500.0;
	//double maxVal = 500.0;
	//should use GetScalarRange()[1], but we dont have an image yet,
	//and this code dont ever get run... pick a value (TF's set from ssc::Image)
	//double maxVal = 1296.0;

	double maxVal = 255;
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
}

VolumetricRepPtr VolumetricRep::New(const std::string& uid, const std::string& name)
{
	VolumetricRepPtr retval(new VolumetricRep(uid, name));
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

/**set a resample factor 0...1. This gives a full-detail image for factor=1,
 * and a more grained image otherwise.
 */
void VolumetricRep::setResampleFactor(double factor)
{
	mResampleFactor = factor;
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
		mImage->disconnectFromRep(mSelf);
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		//disconnect(this, SIGNAL(addPermanentPoint(double, double, double)),
		//			mImage.get(), SLOT(addLandmarkSlot(double, double, double)));
	}

	mImage = image;

	if (mImage)
	{
		mImage->connectToRep(mSelf);
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
	return (mImage == image);
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
	mVolumeProperty->SetColor(mImage->getTransferFunctions3D()->getColorTF());
	mVolumeProperty->SetScalarOpacity(mImage->getTransferFunctions3D()->getOpacityTF());

	// use the base instead of the ref image, because otherwise changes in the transform
	// causes data to be sent anew to the graphics card (takes 4s).
	// changing the mVolume transform instead is a fast operation.
	//
	// also use grayscale as vtk is incapable of rendering 3component color.
	vtkImageDataPtr volume = mImage->getGrayScaleBaseVtkImageData();
	//vtkImageDataPtr volume = mImage->getVOIExtractVtkImageData();

	if (fabs(1.0-mResampleFactor)>0.01) // resampling
	{
		vtkImageResamplePtr resampler = vtkImageResamplePtr::New();
		resampler->SetAxisMagnificationFactor(0, mResampleFactor);
		resampler->SetAxisMagnificationFactor(1, mResampleFactor);
		resampler->SetAxisMagnificationFactor(2, mResampleFactor);
		resampler->SetInput(volume);
		resampler->GetOutput()->Update();
		resampler->GetOutput()->GetScalarRange();
		volume = resampler->GetOutput();
	}




	mTextureMapper3D->SetInput(volume);

	transformChangedSlot();
}

/**called when transform is changed
 * reset it in the prop.*/
void VolumetricRep::transformChangedSlot()
{
	if (!mImage)
	{
		return;
	}
	mVolume->SetUserMatrix(mImage->get_rMd().matrix());
}


//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------
