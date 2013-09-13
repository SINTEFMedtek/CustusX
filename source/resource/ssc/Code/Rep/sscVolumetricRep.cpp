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

#include "sscView.h"
#include "sscImage.h"
#include "sscImageTF3D.h"
#include "sscSlicePlaneClipper.h"
#include "sscTypeConversions.h"
#include "vtkForwardDeclarations.h"
#include "sscMessageManager.h"
#include "cxImageMapperMonitor.h"

#include "cxVolumeProperty.h"

typedef vtkSmartPointer<class vtkGPUVolumeRayCastMapper> vtkGPUVolumeRayCastMapperPtr;

namespace ssc
{
VolumetricRep::VolumetricRep() :
	VolumetricBaseRep(),
	mVolume(vtkVolumePtr::New()),
	mVolumeProperty(cx::VolumeProperty::create()),
	mMaxVoxels(0)
{
	mResampleFactor = 1.0;
	this->setUseVolumeTextureMapper();
	mVolume->SetProperty(mVolumeProperty->getVolumeProperty());
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
		mVolumeProperty->setImage(ssc::ImagePtr());
		mImage->disconnectFromRep(mSelf);
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		mMonitor.reset();
		mMapper->SetInput( (vtkImageData*)NULL );
	}

	mImage = image;

	if (mImage)
	{
		mImage->connectToRep(mSelf);
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		mVolumeProperty->setImage(mImage);
		this->vtkImageDataChangedSlot();
		mMonitor.reset(new cx::ImageMapperMonitor(mVolume, mImage));
		emit internalVolumeChanged();
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
		return factor;
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
	vtkImageDataPtr volume = mImage->resample(this->mResampleFactor);
	mMapper->SetInput(volume);

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

void VolumetricRep::setMaxVolumeSize(long maxVoxels)
{
	mMaxVoxels = maxVoxels;
}


//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------
