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

#include "sscVolumetricRep.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeTextureMapper3D.h>

#if VTK_MINOR_VERSION >= 6
	#include <vtkGPUVolumeRayCastMapper.h>
#endif

#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkImageResample.h>

#include "sscView.h"
#include "sscImage.h"
#include "sscImageTF3D.h"
#include "sscSlicePlaneClipper.h"
#include "sscTypeConversions.h"
#include "vtkForwardDeclarations.h"
#include "sscMessageManager.h"

typedef vtkSmartPointer<class vtkGPUVolumeRayCastMapper> vtkGPUVolumeRayCastMapperPtr;

namespace ssc
{
VolumetricRep::VolumetricRep(const QString& uid, const QString& name) :
	VolumetricBaseRep(uid, name),
	mOpacityTransferFunction(vtkPiecewiseFunctionPtr::New()),
	mColorTransferFunction(vtkColorTransferFunctionPtr::New()),
	mVolumeProperty(vtkVolumePropertyPtr::New()),
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

	this->setUseVolumeTextureMapper();
//	vtkGPUVolumeRayCastMapperPtr mapper = vtkGPUVolumeRayCastMapperPtr::New();
//	//vtkVolumeTextureMapper3DPtr mapper = vtkVolumeTextureMapper3DPtr::New();
//	mMapper = mapper;
//
//	// from snws
//	//mapper->SetPreferredMethodToNVidia();
//	//mTextureMapper3D->SetPreferredMethodToFragmentProgram();
//	mMapper->SetBlendModeToComposite();

 // mTextureMapper3D->CroppingOff();

	mVolume->SetProperty( mVolumeProperty );
//	mVolume->SetMapper( mMapper );


//	if (!mTextureMapper3D->IsRenderSupported(mVolumeProperty))
//	{
//	  std::cout << "Warning: texture rendering not supported" << std::endl;
//	}
}

VolumetricRep::~VolumetricRep()
{}

void VolumetricRep::setUseGPUVolumeRayCastMapper()
{
#if (( VTK_MINOR_VERSION >= 6 )||( VTK_MAJOR_VERSION >5 ))
	vtkGPUVolumeRayCastMapperPtr mapper = vtkGPUVolumeRayCastMapperPtr::New();
	mMapper = mapper;
	mMapper->SetBlendModeToComposite();
	mVolume->SetMapper( mMapper );
#endif
}

void VolumetricRep::setUseVolumeTextureMapper()
{
	vtkVolumeTextureMapper3DPtr mapper = vtkVolumeTextureMapper3DPtr::New();
	mMapper = mapper;

	// from snws
	mapper->SetPreferredMethodToNVidia();
	mMapper->SetBlendModeToComposite();

	mVolume->SetMapper( mMapper );
}

VolumetricRepPtr VolumetricRep::New(const QString& uid, const QString& name)
{
	VolumetricRepPtr retval(new VolumetricRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

void VolumetricRep::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddVolume(mVolume);
}

void VolumetricRep::removeRepActorsFromViewRenderer(View *view)
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
		mMapper->SetInput( (vtkImageData*)NULL );
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
	factor = pow(factor, 1.0/3.0);
	// cubic function leads to trouble for 138M-volume - must downsample to as low as 5-10 Mv in order to succeed on Mac.

	if (factor<0.99)
	{
		//std::cout << "Downsampling volume in VolumetricRep: " << image->getName() << " below " << maxVoxels/1000/1000 << "M. Ratio: " << factor << ", original size: " << voxels/1000/1000 << "M" << std::endl;
		return factor;
		//return 1.0;
	}
	return 1.0;
}

void VolumetricRep::updateResampleFactor()
{
	mResampleFactor = std::min(computeResampleFactor(mMaxVoxels, mImage), mResampleFactor);
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
		resampler->SetInterpolationModeToLinear();
		resampler->SetAxisMagnificationFactor(0, mResampleFactor);
		resampler->SetAxisMagnificationFactor(1, mResampleFactor);
		resampler->SetAxisMagnificationFactor(2, mResampleFactor);
		resampler->SetInput(volume);
		resampler->GetOutput()->Update();
		resampler->GetOutput()->GetScalarRange();
		volume = resampler->GetOutput();

		long voxelsDown = volume->GetNumberOfPoints();
		long voxelsOrig = mImage->getBaseVtkImageData()->GetNumberOfPoints();
		messageManager()->sendInfo("Completed downsampling volume in VolumetricRep: "
								   + mImage->getName()
								   + " below " + qstring_cast(voxelsDown/1000/1000) + "M. "
								   + "Ratio: " + QString::number(mResampleFactor, 'g', 2) + ", "
								   + "Original size: " + qstring_cast(voxelsOrig/1000/1000) + "M.");
	}

	mMapper->SetInput(volume);

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
	mVolume->SetUserMatrix(mImage->get_rMd().getVtkMatrix());
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
