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
#include "sscImage.h"
#include "sscImageTF3D.h"
#include "sscSlicePlaneClipper.h"

typedef vtkSmartPointer<class vtkImageResample> vtkImageResamplePtr;

namespace ssc
{
VolumetricRep::VolumetricRep(const std::string& uid, const std::string& name) :
	VolumetricBaseRep(uid, name),
	mOpacityTransferFunction(vtkPiecewiseFunctionPtr::New()),
	mColorTransferFunction(vtkColorTransferFunctionPtr::New()),
	mVolumeProperty(vtkVolumePropertyPtr::New()),
	mTextureMapper3D(vtkVolumeTextureMapper3DPtr::New()),
	mVolume(vtkVolumePtr::New()),
	mMaxVoxels(0)
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
//	mVolumeProperty->SetInterpolationTypeToNearest();

	// from snws
	mTextureMapper3D->SetPreferredMethodToNVidia();
	//mTextureMapper3D->SetPreferredMethodToFragmentProgram();
	mTextureMapper3D->SetBlendModeToComposite();

 // mTextureMapper3D->CroppingOff();

	mVolume->SetProperty( mVolumeProperty );
	mVolume->SetMapper( mTextureMapper3D );

//	if (!mTextureMapper3D->IsRenderSupported(mVolumeProperty))
//	{
//	  std::cout << "Warning: texture rendering not supported" << std::endl;
//	}

}
VolumetricRep::~VolumetricRep()
{}
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
		disconnect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
		//disconnect(this, SIGNAL(addPermanentPoint(double, double, double)),
		//			mImage.get(), SLOT(addLandmarkSlot(double, double, double)));
		mMonitor.reset();
	}

	mImage = image;

	if (mImage)
	{
		mImage->connectToRep(mSelf);
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
		vtkImageDataChangedSlot();
		mMonitor.reset(new ImageMapperMonitor(mVolume, mImage));
		emit internalVolumeChanged();
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

double VolumetricRep::computeResampleFactor(long maxVoxels, ssc::ImagePtr image)
{
	if (maxVoxels==0)
		return 1.0;
	if (!image)
		return 1.0;

	long voxels = image->getBaseVtkImageData()->GetNumberOfPoints();
	double factor = (double)maxVoxels/(double)voxels;
	//factor = pow(factor, 1.0/3.0); did not work. Seems that the resampling is linear?
	if (factor<0.99)
	{
		std::cout << "Downsampling volume in VolumetricRep: " << image->getName() << " below " << maxVoxels/1000/1000 << "M. Ratio: " << factor << ", original size: " << voxels/1000/1000 << "M" << std::endl;
		return factor;
	}	
	return 1.0;
}

void VolumetricRep::updateResampleFactor()
{
	mResampleFactor = std::min(computeResampleFactor(mMaxVoxels, mImage), mResampleFactor);
<<<<<<< HEAD:Code/Rep/sscVolumetricRep.cpp
//	
//	if (mMaxVoxels==0)
//		return;
//
//	long voxels = mImage->getBaseVtkImageData()->GetNumberOfPoints();
//	double factor = (double)mMaxVoxels/(double)voxels;
//	//factor = pow(factor, 1.0/3.0); did not work. Seems that the resampling is linear?
//	if (factor<0.99)
//	{
//		std::cout << "Downsampling volume in VolumetricRep: " << mImage->getName() << " below " << mMaxVoxels/1000/1000 << "M. Ratio: " << factor << ", original size: " << voxels/1000/1000 << "M" << std::endl;
//		mResampleFactor = std::min(factor, mResampleFactor);
//	}
=======
>>>>>>> fd321bd6dbdad0c83f5180274670284ded7925cb:Code/Rep/sscVolumetricRep.cpp
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

	this->updateResampleFactor();

	//mVolumeProperty->SetColor(mImage->getTransferFunctions3D()->getColorTF());
	//mVolumeProperty->SetScalarOpacity(mImage->getTransferFunctions3D()->getOpacityTF());

	// also use grayscale as vtk is incapable of rendering 3component color.
	vtkImageDataPtr volume = mImage->getGrayScaleBaseVtkImageData();

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

	transferFunctionsChangedSlot();
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

void VolumetricRep::transferFunctionsChangedSlot()
{
  mVolumeProperty->SetColor(mImage->getTransferFunctions3D()->getColorTF());
  mVolumeProperty->SetScalarOpacity(mImage->getTransferFunctions3D()->getOpacityTF());
  mVolumeProperty->SetShade(mImage->getShadingOn());
  
  //Shading parameters from OsiriX
  /*mVolumeProperty->SetAmbient(0.15);
  mVolumeProperty->SetDiffuse(0.90);
  mVolumeProperty->SetSpecular(0.30);
  mVolumeProperty->SetSpecularPower(15.00);*/
  
  mVolumeProperty->SetAmbient(mImage->getShadingAmbient());
  mVolumeProperty->SetDiffuse(mImage->getShadingDiffuse());
  mVolumeProperty->SetSpecular(mImage->getShadingSpecular());
  mVolumeProperty->SetSpecularPower(mImage->getShadingSpecularPower());
  
}

void VolumetricRep::setMaxVolumeSize(long maxVoxels)
{
	mMaxVoxels = maxVoxels;
}


//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------
