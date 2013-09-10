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

#include "cxMultiVolume3DRepProducer.h"
#include "sscMessageManager.h"
#include "sscVolumetricRep.h"
#include "sscProgressiveLODVolumetricRep.h"
#include <vtkImageData.h>
#include "sscImage2DRep3D.h"
#include "sscView.h"
#include "sscTypeConversions.h"
#include "sscLogger.h"
#include "sscGPURayCastVolumeRep.h"
#include "cxDataLocations.h"
#include "cxMehdiGPURayCastMultiVolumeRep.h"
#include "cxConfig.h"

namespace cx
{


MultiVolume3DRepProducer::MultiVolume3DRepProducer()
{
	mView = NULL;
	mMaxRenderSize = 10 * pow(10.0,6);
}

void MultiVolume3DRepProducer::setView(ssc::View* view)
{
	if (view==mView)
		return;
	this->clearReps();
	mView = view;
	this->fillReps();
}

QStringList MultiVolume3DRepProducer::getAvailableVisualizers()
{
	QStringList retval;
	retval << "vtkVolumeTextureMapper3D";
#if !defined(__APPLE__) && !defined(WIN32)
	retval << "vtkGPUVolumeRayCastMapper";
#endif
	retval << "sscProgressiveLODVolumeTextureMapper3D";
	retval << "sscGPURayCastMultiVolume";	
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
	retval << "vtkOpenGLGPUMultiVolumeRayCastMapper";
#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME

	return retval;
}

std::map<QString, QString> MultiVolume3DRepProducer::getAvailableVisualizerDisplayNames()
{
	std::map<QString, QString> names;
	names["vtkVolumeTextureMapper3D"] = "Texture (single volume)";
	names["vtkGPUVolumeRayCastMapper"] = "Raycast GPU (single volume)";
	names["sscProgressiveLODVolumeTextureMapper3D"] = "Progressive texture (single volume)";
	names["sscGPURayCastMultiVolume"] = "Snw Raycast GPU (multi volume)";
	names["vtkOpenGLGPUMultiVolumeRayCastMapper"] = "Mehdi Raycast GPU (multi volume)";
	return names;
}

void MultiVolume3DRepProducer::setMaxRenderSize(int voxels)
{
	mMaxRenderSize = voxels;
	if (mMaxRenderSize<1)
		mMaxRenderSize = 10 * pow(10.0,6);

	this->updateRepsInView();
}

int MultiVolume3DRepProducer::getMaxRenderSize() const
{
	return mMaxRenderSize;
}

void MultiVolume3DRepProducer::setVisualizerType(QString type)
{
	mVisualizerType = type;

	this->updateRepsInView();
}

void MultiVolume3DRepProducer::addImage(ssc::ImagePtr image)
{
	if (image)
	{
		connect(image.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(imagesChanged()));
		connect(image.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(imagesChanged()));
	}

	mImages.push_back(image);
	emit imagesChanged();

	this->updateRepsInView();
}

void MultiVolume3DRepProducer::removeImage(QString uid)
{
	ssc::ImagePtr image;
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		if (mImages[i]->getUid()!=uid)
			continue;
		image = mImages[i];
		mImages.erase(mImages.begin()+i);
		break;
	}

	if (image)
	{
		disconnect(image.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(imagesChanged()));
		disconnect(image.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(imagesChanged()));
	}

	emit imagesChanged();
	this->updateRepsInView();
}

std::vector<ssc::RepPtr> MultiVolume3DRepProducer::getAllReps()
{
	return mReps;
}

void MultiVolume3DRepProducer::updateRepsInView()
{
	this->clearReps();
	this->fillReps();
}

void MultiVolume3DRepProducer::clearReps()
{
	this->removeRepsFromView();
	mReps.clear();
}

void MultiVolume3DRepProducer::removeRepsFromView()
{
	if (!mView)
		return;

	for (unsigned i=0; i<mReps.size(); ++i)
		mView->removeRep(mReps[i]);
}

void MultiVolume3DRepProducer::fillReps()
{
	this->rebuildReps();
	this->addRepsToView();
}

void MultiVolume3DRepProducer::addRepsToView()
{
	if (!mView)
		return;

	for (unsigned i=0; i<mReps.size(); ++i)
		mView->addRep(mReps[i]);
}

void MultiVolume3DRepProducer::rebuildReps()
{
	if (mImages.empty())
		return;

	if (this->isSingleVolumeRenderer())
	{
		for (unsigned i=0; i<mImages.size(); ++i)
			this->buildSingleVolumeRenderer(mImages[i]);
	}
	else if (mVisualizerType=="sscGPURayCastMultiVolume")
	{
		this->buildSscGPURayCastMultiVolume();
	}
	else if (mVisualizerType=="vtkOpenGLGPUMultiVolumeRayCastMapper")
	{
		this->buildVtkOpenGLGPUMultiVolumeRayCastMapper();
	}
	else
	{
		ssc::messageManager()->sendError(QString("No visualizer found for string=%1").arg(mVisualizerType));
	}
}

void MultiVolume3DRepProducer::buildSscGPURayCastMultiVolume()
{
#ifndef CX_WINDOWS
	ssc::GPURayCastVolumeRepPtr rep = ssc::GPURayCastVolumeRep::New("");
	rep->setShaderFolder(DataLocations::getShaderPath());
	rep->setImages(mImages);
	mReps.push_back(rep);
#endif //WIN32
}

void MultiVolume3DRepProducer::buildVtkOpenGLGPUMultiVolumeRayCastMapper()
{
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
	MehdiGPURayCastMultiVolumeRepPtr rep = MehdiGPURayCastMultiVolumeRep::New("");
	rep->setImages(mImages);
	mReps.push_back(rep);
#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
}

bool MultiVolume3DRepProducer::is2DImage(ssc::ImagePtr image) const
{
	return image->getBaseVtkImageData()->GetDimensions()[2]==1;
}

void MultiVolume3DRepProducer::buildSingleVolumeRenderer(ssc::ImagePtr image)
{
	if (this->is2DImage(image))
	{
		this->buildSscImage2DRep3D(image);
	}
	else if (mVisualizerType=="vtkVolumeTextureMapper3D")
	{
		this->buildVtkVolumeTextureMapper3D(image);
	}
	else if (mVisualizerType=="vtkGPUVolumeRayCastMapper")
	{
		this->buildVtkGPUVolumeRayCastMapper(image);
	}
	else if (mVisualizerType=="sscProgressiveLODVolumeTextureMapper3D")
	{
		this->buildSscProgressiveLODVolumeTextureMapper3D(image);
	}
	else
	{
		ssc::messageManager()->sendError(QString("No visualizer found for string=%1").arg(mVisualizerType));
		return;
	}
}

bool MultiVolume3DRepProducer::isSingleVolumeRenderer() const
{
	QStringList singleTypes;
	singleTypes << "vtkVolumeTextureMapper3D" << "vtkGPUVolumeRayCastMapper" << "sscProgressiveLODVolumeTextureMapper3D";
	return singleTypes.count(mVisualizerType);
}

void MultiVolume3DRepProducer::buildSscImage2DRep3D(ssc::ImagePtr image)
{
	cx::Image2DRep3DPtr rep = cx::Image2DRep3D::New();
	rep->setImage(image);
	mReps.push_back(rep);
}

void MultiVolume3DRepProducer::buildVtkVolumeTextureMapper3D(ssc::ImagePtr image)
{
	ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New();
	rep->setUseVolumeTextureMapper();

	rep->setMaxVolumeSize(this->getMaxRenderSize());
	rep->setImage(image);
	mReps.push_back(rep);
}

void MultiVolume3DRepProducer::buildVtkGPUVolumeRayCastMapper(ssc::ImagePtr image)
{
	ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New();
	rep->setUseGPUVolumeRayCastMapper();

	rep->setMaxVolumeSize(this->getMaxRenderSize());
	rep->setImage(image);
	mReps.push_back(rep);
}

void MultiVolume3DRepProducer::buildSscProgressiveLODVolumeTextureMapper3D(ssc::ImagePtr image)
{
	ssc::ProgressiveLODVolumetricRepPtr rep = ssc::ProgressiveLODVolumetricRep::New();

	rep->setMaxVolumeSize(this->getMaxRenderSize());
	rep->setImage(image);
	mReps.push_back(rep);
}



} // namespace cx
