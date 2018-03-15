/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

#include "cxMehdiGPURayCastMultiVolumeRep.h"
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include <vtkVolumeProperty.h>
#include "cxView.h"
#include <vtkRenderer.h>
#include "cxVolumeProperty.h"
#include "cxImageMapperMonitor.h"
#include "cxImage.h"
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include "cxGPUImageBuffer.h"
#include "cxReporter.h"
#include <vtkPlane.h>
#include "cxLogger.h"
#include "cxImageEnveloper.h"
#include "cxTypeConversions.h"
#include <vtkRenderWindow.h>

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
	mMaxVoxels(10*1000*1000)
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

	this->disconnectImages();
	this->clearVolume();

	mImages = images;

	this->setupVolume();
	this->connectImages();
}

void MehdiGPURayCastMultiVolumeRep::clearVolume()
{
	mMonitors.clear();
	mVolumeProperties.clear();

	// should not be necessary, but seems that mapper is not cleared by vtk.
	// http://vtk.1045678.n5.nabble.com/quot-a-vtkShader2-object-is-being-deleted-before-ReleaseGraphicsResources-has-been-called-quot-with-r-td4872396.html
	if (this->getView())
		mVolume->ReleaseGraphicsResources(this->getView()->getRenderWindow());
	mVolume->SetMapper(NULL);

	mReferenceImage.reset();
	mReferenceProperty.reset();
	mMapper = NULL;
}

void MehdiGPURayCastMultiVolumeRep::vtkImageDataChangedSlot()
{
	// this call is extremly slow, as the entire rep is rebuilt (up to 5 seconds). Solution is to optimize/repalce mapper.
	this->clearVolume();
	this->setupVolume();
}

void MehdiGPURayCastMultiVolumeRep::transformChangedSlot()
{
	this->vtkImageDataChangedSlot();

	// no good: must call entire setupVolume() during init of mapper.
//	this->setupReferenceVolumeAndPropertiesAndConnectToVolume();
//	this->updateTransforms();
}


// use in setupVolume:
//		// example code for how to allocate on gpu and return uid:
//		GPUImageDataBufferPtr dataBuffer = GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(
//			mImages[i]->getBaseVtkImageData());
		// crashes: must probably initialize gl context.
//		dataBuffer->allocate();
//		unsigned int glUint = dataBuffer->getTextureUid();


void MehdiGPURayCastMultiVolumeRep::setupVolume()
{
	if (mImages.empty())
		return;

	this->setupVolumeProperties();

	this->initializeMapper();

	for (unsigned i=0; i<mImages.size(); ++i)
	{
		// index starts with main volume (and continues into additionalVolumes()), thus +1
		mMapper->SetInput(i+1, mImages[i]->getBaseVtkImageData());
		mMapper->SetAdditionalProperty(i, mVolumeProperties[i]->getVolumeProperty() );//Mehdi
	}

	this->setupReferenceVolumeAndPropertiesAndConnectToVolume();

	this->updateTransforms();

	// call after mVolume has been initialized
	this->setupMonitor();

	mMapper->Update();
}

void MehdiGPURayCastMultiVolumeRep::initializeMapper()
{
	mMapper = vtkOpenGLGPUMultiVolumeRayCastMapperPtr::New();
	mMapper->setNumberOfAdditionalVolumes(mImages.size());
	mVolume->SetMapper(mMapper);
}


void MehdiGPURayCastMultiVolumeRep::setupReferenceVolumeAndPropertiesAndConnectToVolume()
{
	SSC_ASSERT(!mImages.empty());

	mReferenceImage = this->getEnvelopingImage();
	mReferenceProperty = VolumeProperty::create();
	// hack: use properties from first input image.
	// This is because some properties (at least shading) is taken from here.
	mReferenceProperty->setImage(mImages[0]);
	mVolume->SetProperty( mReferenceProperty->getVolumeProperty() );
	mMapper->SetInput(0, mReferenceImage->getBaseVtkImageData());
}

void MehdiGPURayCastMultiVolumeRep::updateTransforms()
{
	if (mImages.empty())
		return;

	SSC_ASSERT(mReferenceImage);

	Transform3D rMd0 = mReferenceImage->get_rMd(); // use on first volume
	mVolume->SetUserMatrix(rMd0.getVtkMatrix());

	for (unsigned i=0; i<mImages.size(); ++i)
	{
		Transform3D rMdi = mImages[i]->get_rMd();
		Transform3D d0Mdi = rMd0.inv() * rMdi; // use on additional volumes

		mMapper->SetAdditionalInputUserTransform(i, d0Mdi.getVtkTransform());
	}
}

ImagePtr MehdiGPURayCastMultiVolumeRep::getEnvelopingImage()
{
	ImageEnveloperPtr generator;
	generator = ImageEnveloper::create();
	generator->setImages(mImages);
	generator->setMaxEnvelopeVoxels(mMaxVoxels);
	return generator->getEnvelopingImage();
}

void MehdiGPURayCastMultiVolumeRep::disconnectImages()
{
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		disconnect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		disconnect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
	}
}

void MehdiGPURayCastMultiVolumeRep::connectImages()
{
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
	}
}

void MehdiGPURayCastMultiVolumeRep::setupMonitor()
{
	mMonitors.resize(mImages.size());
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		mMonitors[i] = MehdiGPURayCastMultiVolumeRepImageMapperMonitor::create(mVolume, mImages[i], i+1);
	}
}

void MehdiGPURayCastMultiVolumeRep::setupVolumeProperties()
{
	mVolumeProperties.clear();
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		VolumePropertyPtr property = VolumeProperty::create();
		property->setImage(mImages[i]);
		mVolumeProperties.push_back(property);
	}
}

} // namespace cx

#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
