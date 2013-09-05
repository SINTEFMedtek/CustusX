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

namespace cx
{


MultiVolume3DRepProducer::MultiVolume3DRepProducer()
{
	mMaxRenderSize = 10 * pow(10.0,6);
}

void MultiVolume3DRepProducer::setMaxRenderSize(int voxels)
{
	mMaxRenderSize = voxels;
}

int MultiVolume3DRepProducer::getMaxRenderSize() const
{
	return mMaxRenderSize;
}

void MultiVolume3DRepProducer::setVisualizerType(QString type)
{
	mVisualizerType = type;
}

void MultiVolume3DRepProducer::addImage(ssc::ImagePtr image)
{
	mImages.push_back(image);
	emit imagesChanged();

	this->clearReps();
}

void MultiVolume3DRepProducer::removeImage(QString uid)
{
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		if (mImages[i]->getUid()!=uid)
			continue;
		mImages.erase(mImages.begin()+i);
		break;
	}
	emit imagesChanged();
	this->clearReps();
}

void MultiVolume3DRepProducer::clearReps()
{
	mReps.clear();
	emit repsChanged();
}

std::vector<ssc::RepPtr> MultiVolume3DRepProducer::getAllReps()
{
	if (mReps.empty())
		this->rebuildReps();
	return mReps;
}

void MultiVolume3DRepProducer::rebuildReps()
{
	mReps.clear();

	if (this->isSingleVolumeRenderer())
	{
		for (unsigned i=0; i<mImages.size(); ++i)
			this->buildSingleVolumeRenderer(mImages[i]);
	}
	else
	{
		ssc::messageManager()->sendError(QString("No visualizer found for string=%1").arg(mVisualizerType));
		return;
	}
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
