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

namespace cx
{



MehdiGPURayCastMultiVolumeRep::~MehdiGPURayCastMultiVolumeRep()
{

}

MehdiGPURayCastMultiVolumeRep::MehdiGPURayCastMultiVolumeRep() :
	mVolume(vtkVolumePtr::New())
{
}

void MehdiGPURayCastMultiVolumeRep::addRepActorsToViewRenderer(ssc::View* view)
{
	view->getRenderer()->AddVolume(mVolume);
}

void MehdiGPURayCastMultiVolumeRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	view->getRenderer()->RemoveVolume(mVolume);
}

void MehdiGPURayCastMultiVolumeRep::setImages(std::vector<ssc::ImagePtr> images)
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

void MehdiGPURayCastMultiVolumeRep::setup()
{
	mMonitors.resize(mImages.size());

	if (mImages.empty())
		return;

	mMapper = vtkOpenGLGPUMultiVolumeRayCastMapperPtr::New();
	mMapper->setNumberOfAdditionalVolumes(mImages.size()-1);
	mVolume->SetMapper(mMapper);

	this->transformChangedSlot();

	for (unsigned i=0; i<mImages.size(); ++i)
	{
		VolumePropertyPtr property = VolumeProperty::create();
		property->setImage(mImages[i]);
		mVolumeProperties.push_back(property);

		mMapper->SetInput(i, mImages[i]->getBaseVtkImageData());

		if (i==0)
			mVolume->SetProperty( property->getVolumeProperty() );
		else if (i>0)
			mMapper->SetAdditionalProperty(i-1, property->getVolumeProperty() );//Mehdi

	}

	// call after mVolume has been initialized
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		mMonitors[i].reset(new ImageMapperMonitor(mVolume, mImages[i]));
	}

	mMapper->Update();
}

void MehdiGPURayCastMultiVolumeRep::transformChangedSlot()
{
	if (mImages.empty())
		return;

	ssc::Transform3D rMd0 = mImages[0]->get_rMd(); // use on first volume

	for (unsigned i=0; i<mImages.size(); ++i)
	{
		ssc::Transform3D rMdi = mImages[i]->get_rMd();
		ssc::Transform3D d0Mdi = rMd0.inv() * rMdi; // use on additional volumescd

		if (i==0)
		{
			mVolume->SetUserMatrix(rMd0.getVtkMatrix());
		}
		else if (i>0)
		{
			vtkTransformPtr transform = vtkTransform::New();
			transform->SetMatrix(d0Mdi.getVtkMatrix());
			transform->Update();
			mMapper->SetAdditionalInputUserTransform(i-1,transform); //Mehdi
		}

	}
}

} // namespace cx

#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
