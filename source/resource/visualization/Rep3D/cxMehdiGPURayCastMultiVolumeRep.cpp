// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

#include "cxMehdiGPURayCastMultiVolumeRep.h"
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include <vtkVolumeProperty.h>
#include "sscView.h"
#include <vtkRenderer.h>
#include "cxVolumeProperty.h"
#include "cxImageMapperMonitor.h"
#include "sscImage.h"
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include "sscGPUImageBuffer.h"
#include "sscMessageManager.h"
#include <vtkPlane.h>
#include "sscLogger.h"
#include "cxImageEnveloper.h"

namespace cx
{

ImageMapperMonitorPtr MehdiGPURayCastMultiVolumeRepImageMapperMonitor::create(vtkVolumePtr volume, ImagePtr image, int volumeIndex)
{
	MehdiGPURayCastMultiVolumeRepImageMapperMonitor* retval = new MehdiGPURayCastMultiVolumeRepImageMapperMonitor(volume, image, volumeIndex);
	retval->init(); // contains virtual functions
	return ImageMapperMonitorPtr(retval);
}

MehdiGPURayCastMultiVolumeRepImageMapperMonitor::MehdiGPURayCastMultiVolumeRepImageMapperMonitor(vtkVolumePtr volume, ImagePtr image, int volumeIndex) :
	ImageMapperMonitor(volume,image), mVolumeIndex(volumeIndex)
{
}

vtkOpenGLGPUMultiVolumeRayCastMapperPtr MehdiGPURayCastMultiVolumeRepImageMapperMonitor::getMehdiMapper()
{
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mapper;
	mapper = dynamic_cast<vtkOpenGLGPUMultiVolumeRayCastMapper*>(mVolume->GetMapper());
	return mapper;
}

void MehdiGPURayCastMultiVolumeRepImageMapperMonitor::clearClipping()
{
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mapper = this->getMehdiMapper();
	if (!mapper)
		return;
	mapper->RemoveClippingPlane(mVolumeIndex);
}

void MehdiGPURayCastMultiVolumeRepImageMapperMonitor::applyClipping()
{
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mapper = this->getMehdiMapper();
	if (!mapper)
		return;
	std::vector<vtkPlanePtr> planes = mImage->getAllClipPlanes();
	if (planes.empty())
	{
		mapper->RemoveClippingPlane(mVolumeIndex);
		return;
	}
	else
	{
		mapper->AddClippingPlane(mVolumeIndex, planes[0]);
	}
}

void MehdiGPURayCastMultiVolumeRepImageMapperMonitor::applyCropping()
{
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mapper = this->getMehdiMapper();
	if (!mapper)
		return;

	mapper->SetCropping(mVolumeIndex, mImage->getCropping());

	DoubleBoundingBox3D bb_d = mImage->getCroppingBox();

	mapper->SetCroppingRegionPlanes(mVolumeIndex, bb_d.begin());
	mapper->Update();
}


////////////////////////////////////////////////////

MehdiGPURayCastMultiVolumeRepBase::MehdiGPURayCastMultiVolumeRepBase() :
	mMaxVoxels(0)
{
}

void MehdiGPURayCastMultiVolumeRepBase::setMaxVolumeSize(long maxVoxels)
{
	mMaxVoxels = maxVoxels;
}

///////////////////////////////////////////////////



MehdiGPURayCastMultiVolumeRep::~MehdiGPURayCastMultiVolumeRep()
{

}

MehdiGPURayCastMultiVolumeRep::MehdiGPURayCastMultiVolumeRep() :
	mVolume(vtkVolumePtr::New())
{
	mGenerator = ImageEnveloperImpl::create();
}

void MehdiGPURayCastMultiVolumeRep::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddVolume(mVolume);
}

void MehdiGPURayCastMultiVolumeRep::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveVolume(mVolume);
}

void MehdiGPURayCastMultiVolumeRep::setImages(std::vector<ImagePtr> images)
{
	if (images==mImages)
		return;

	this->clear();

	mImages = images;

	this->setup();
}

void MehdiGPURayCastMultiVolumeRep::clear()
{
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		disconnect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		disconnect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
	}
	mMonitors.clear();
	mImages.clear();

	mVolumeProperties.clear();
	mVolume->SetMapper(NULL);
}

void MehdiGPURayCastMultiVolumeRep::vtkImageDataChangedSlot()
{
	this->clear();
	this->setup();
}

void MehdiGPURayCastMultiVolumeRep::setBoundingBoxGenerator(ImageEnveloperPtr generator)
{
	mGenerator = generator;
}

void MehdiGPURayCastMultiVolumeRep::setup()
{
	mMonitors.resize(mImages.size());

	if (mImages.empty())
		return;

	mGenerator->setImages(mImages);
	//mGenerator->setMaxVoxelSize(...);
//	MultiVolumeBoundingBoxGenerator::Box box = mGenerator->getBox();
	mReferenceImage = mGenerator->getEnvelopingImage(mMaxVoxels);

	mMapper = vtkOpenGLGPUMultiVolumeRayCastMapperPtr::New();
	mMapper->setNumberOfAdditionalVolumes(mImages.size());
	mVolume->SetMapper(mMapper);

	this->transformChangedSlot();

	mMapper->SetInput(0, mReferenceImage->getBaseVtkImageData());
	//mVolume->SetProperty( property->getVolumeProperty() );

	for (unsigned i=0; i<mImages.size(); ++i)
	{
		VolumePropertyPtr property = VolumeProperty::create();
		property->setImage(mImages[i]);
		mVolumeProperties.push_back(property);

//		// example code for how to allocate on gpu and return uid:
//		GPUImageDataBufferPtr dataBuffer = GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(
//			mImages[i]->getBaseVtkImageData());
		// crashes: must probably initialize gl context.
//		dataBuffer->allocate();
//		unsigned int glUint = dataBuffer->getTextureUid();

		// index starts with main volume (and continues into additionalVolumes()), thus +1
		mMapper->SetInput(i+1, mImages[i]->getBaseVtkImageData());

//		if (i==0)
//			mVolume->SetProperty( property->getVolumeProperty() );
//		else if (i>0)
//			mMapper->SetAdditionalProperty(i-1, property->getVolumeProperty() );//Mehdi
		mMapper->SetAdditionalProperty(i, property->getVolumeProperty() );//Mehdi
	}

	// call after mVolume has been initialized
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		mMonitors[i] = MehdiGPURayCastMultiVolumeRepImageMapperMonitor::create(mVolume, mImages[i], i);
	}

	mMapper->Update();
}

void MehdiGPURayCastMultiVolumeRep::transformChangedSlot()
{
	if (mImages.empty())
		return;

	Transform3D rMd0 = mReferenceImage->get_rMd(); // use on first volume
	mVolume->SetUserMatrix(rMd0.getVtkMatrix());

	for (unsigned i=0; i<mImages.size(); ++i)
	{
		Transform3D rMdi = mImages[i]->get_rMd();
		Transform3D d0Mdi = rMd0.inv() * rMdi; // use on additional volumescd

		mMapper->SetAdditionalInputUserTransform(i, d0Mdi.getVtkTransform());
	}
}

} // namespace cx

#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
