/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxVolumetricRep.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>

#ifndef CX_VTK_OPENGL2
//#include <vtkVolumeTextureMapper3D.h>
#else
#include <vtkSmartVolumeMapper.h>
#endif
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
