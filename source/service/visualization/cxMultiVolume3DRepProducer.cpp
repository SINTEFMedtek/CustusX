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
#include "cxReporter.h"
#include "cxVolumetricRep.h"
#include <vtkImageData.h>
#include "cxImage2DRep3D.h"
#include "cxView.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxDataLocations.h"
#include "cxMehdiGPURayCastMultiVolumeRep.h"
#include "cxConfig.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

namespace cx
{


MultiVolume3DRepProducer::MultiVolume3DRepProducer()
{
	mView = NULL;
	mMaxRenderSize = 10 * pow(10.0,6);
}

MultiVolume3DRepProducer::~MultiVolume3DRepProducer()
{
}

void MultiVolume3DRepProducer::setView(View* view)
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
	retval << "vtkGPUVolumeRayCastMapper";
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

bool MultiVolume3DRepProducer::contains(ImagePtr image) const
{
	if (std::count(m2DImages.begin(), m2DImages.end(), image))
		return true;
	if (std::count(m3DImages.begin(), m3DImages.end(), image))
		return true;
	return false;
}

void MultiVolume3DRepProducer::addImage(ImagePtr image)
{
	if (this->contains(image))
		return;

	if (image)
	{
		connect(image.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(imagesChanged()));
		connect(image.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(imagesChanged()));
	}
	else
		return;

	if(this->is2DImage(image))
		m2DImages.push_back(image);
	else
		m3DImages.push_back(image);
	emit imagesChanged();

	this->updateRepsInView();
}

void MultiVolume3DRepProducer::removeImage(QString uid)
{
	ImagePtr removedImage;
	removedImage = this->removeImageFromVector(uid, m2DImages);
	if(!removedImage)
		removedImage = this->removeImageFromVector(uid, m3DImages);

	if (removedImage)
	{
		disconnect(removedImage.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(imagesChanged()));
		disconnect(removedImage.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(imagesChanged()));
	}

	emit imagesChanged();
	this->updateRepsInView();
}

ImagePtr MultiVolume3DRepProducer::removeImageFromVector(QString uid, std::vector<ImagePtr> &images)
{
	ImagePtr retval;
	for (unsigned i=0; i<images.size(); ++i)
	{
		if (images[i]->getUid()!=uid)
			continue;
		retval = images[i];
		images.erase(images.begin()+i);
		break;
	}
	return retval;
}

std::vector<RepPtr> MultiVolume3DRepProducer::getAllReps()
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
	if(!m2DImages.empty())
		this->rebuild2DReps();
	if(!m3DImages.empty())
		this->rebuild3DReps();
}

void MultiVolume3DRepProducer::rebuild2DReps()
{
	for (unsigned i=0; i<m2DImages.size(); ++i)
		this->buildSscImage2DRep3D(m2DImages[i]);
}

void MultiVolume3DRepProducer::rebuild3DReps()
{
	if (this->isSingleVolumeRenderer())
	{
		for (unsigned i=0; i<m3DImages.size(); ++i)
			this->buildSingleVolumeRenderer(m3DImages[i]);
	}
	else if (mVisualizerType=="vtkOpenGLGPUMultiVolumeRayCastMapper")
	{
		this->buildVtkOpenGLGPUMultiVolumeRayCastMapper();
	}
	else
	{
		reportError(QString("No visualizer found for string=%1").arg(mVisualizerType));
	}
}

void MultiVolume3DRepProducer::buildVtkOpenGLGPUMultiVolumeRayCastMapper()
{
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
	MehdiGPURayCastMultiVolumeRepPtr rep = MehdiGPURayCastMultiVolumeRep::New("");
	rep->setMaxVolumeSize(this->getMaxRenderSize());
	rep->setImages(m3DImages);
	mReps.push_back(rep);
#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
}

bool MultiVolume3DRepProducer::is2DImage(ImagePtr image) const
{
	if(image)
		return image->getBaseVtkImageData()->GetDimensions()[2]==1;
	return false;
}

void MultiVolume3DRepProducer::buildSingleVolumeRenderer(ImagePtr image)
{
	if (mVisualizerType=="vtkVolumeTextureMapper3D")
	{
		this->buildVtkVolumeTextureMapper3D(image);
	}
	else if (mVisualizerType=="vtkGPUVolumeRayCastMapper")
	{
		this->buildVtkGPUVolumeRayCastMapper(image);
	}
	else
	{
		reportError(QString("No visualizer found for string=%1").arg(mVisualizerType));
		return;
	}
}

bool MultiVolume3DRepProducer::isSingleVolumeRenderer() const
{
	QStringList singleTypes;
	singleTypes << "vtkVolumeTextureMapper3D" << "vtkGPUVolumeRayCastMapper";
	return singleTypes.count(mVisualizerType);
}

void MultiVolume3DRepProducer::buildSscImage2DRep3D(ImagePtr image)
{
	cx::Image2DRep3DPtr rep = cx::Image2DRep3D::New();
	rep->setImage(image);
	mReps.push_back(rep);
}

void MultiVolume3DRepProducer::buildVtkVolumeTextureMapper3D(ImagePtr image)
{
	VolumetricRepPtr rep = VolumetricRep::New();
	rep->setUseVolumeTextureMapper();

	rep->setMaxVolumeSize(this->getMaxRenderSize());
	rep->setImage(image);
	mReps.push_back(rep);
}

void MultiVolume3DRepProducer::buildVtkGPUVolumeRayCastMapper(ImagePtr image)
{
	VolumetricRepPtr rep = VolumetricRep::New();
	rep->setUseGPUVolumeRayCastMapper();

	rep->setMaxVolumeSize(this->getMaxRenderSize());
	rep->setImage(image);
	mReps.push_back(rep);
}

} // namespace cx
