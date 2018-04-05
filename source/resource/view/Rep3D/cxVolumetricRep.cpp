/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxVolumetricRep.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>

#include <vtkSmartVolumeMapper.h>

//#if VTK_MINOR_VERSION >= 6
	#include <vtkGPUVolumeRayCastMapper.h>
//#endif

#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>

#include "cxView.h"
#include "cxImage.h"
#include "cxImageTF3D.h"
#include "cxSlicePlaneClipper.h"
#include "cxTypeConversions.h"
#include "vtkForwardDeclarations.h"

#include "cxImageMapperMonitor.h"

#include "cxVolumeProperty.h"

typedef vtkSmartPointer<class vtkGPUVolumeRayCastMapper> vtkGPUVolumeRayCastMapperPtr;

namespace cx
{
VolumetricRep::VolumetricRep() :
	VolumetricBaseRep(),
	mVolume(vtkVolumePtr::New()),
	mVolumeProperty(cx::VolumeProperty::create()),
	mMaxVoxels(0)
{
	this->setUseVolumeTextureMapper();
	mVolume->SetProperty(mVolumeProperty->getVolumeProperty());
}

VolumetricRep::~VolumetricRep()
{}

void VolumetricRep::setUseGPUVolumeRayCastMapper()
{
//#if (( VTK_MINOR_VERSION >= 6 )||( VTK_MAJOR_VERSION >5 ))
	vtkGPUVolumeRayCastMapperPtr mapper = vtkGPUVolumeRayCastMapperPtr::New();
	mMapper = mapper;
	mMapper->SetBlendModeToComposite();
	mVolume->SetMapper( mMapper );
//#endif
}

//TODO: Use vtkSmartVolumeMapper instead
void VolumetricRep::setUseVolumeTextureMapper()
{
	vtkVolumeTextureMapper3DPtr mapper = vtkVolumeTextureMapper3DPtr::New();
	mMapper = mapper;

	// from snws
#ifndef CX_VTK_OPENGL2
	mapper->SetPreferredMethodToNVidia();
#endif
	mMapper->SetBlendModeToComposite();

	mVolume->SetMapper( mMapper );
}

void VolumetricRep::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddVolume(mVolume);
}

void VolumetricRep::removeRepActorsFromViewRenderer(ViewPtr view)
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
		mVolumeProperty->setImage(ImagePtr());
		disconnect(mImage.get(), &Image::vtkImageDataChanged, this, &VolumetricRep::vtkImageDataChangedSlot);
		disconnect(mImage.get(), &Image::transformChanged, this, &VolumetricRep::transformChangedSlot);
		mMonitor.reset();
		mMapper->SetInputData( (vtkImageData*)NULL );
	}

	mImage = image;

	if (mImage)
	{
		connect(mImage.get(), &Image::vtkImageDataChanged, this, &VolumetricRep::vtkImageDataChangedSlot);
		connect(mImage.get(), &Image::transformChanged, this, &VolumetricRep::transformChangedSlot);
		mVolumeProperty->setImage(mImage);
		this->vtkImageDataChangedSlot();
		mMonitor = ImageMapperMonitor::create(mVolume, mImage);
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
	this->updateVtkImageDataSlot();
	this->transformChangedSlot();
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

void VolumetricRep::updateVtkImageDataSlot()
{
	if (!mImage)
		return;

	vtkImageDataPtr volume = mImage->resample(this->mMaxVoxels);
	mMapper->SetInputData(volume);
}

void VolumetricRep::setMaxVolumeSize(long maxVoxels)
{
	mMaxVoxels = maxVoxels;
}


//---------------------------------------------------------
} // namespace cx
//---------------------------------------------------------
