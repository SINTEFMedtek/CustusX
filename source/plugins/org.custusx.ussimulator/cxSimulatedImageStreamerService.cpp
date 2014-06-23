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
#include "cxSimulatedImageStreamerService.h"

#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxReporter.h"
#include "cxSimulateUSWidget.h"

//These 3 includes should be removed when TrackingManager are a plugin
#include "cxLogicManager.h"
#include "cxVideoService.h"
#include "cxVideoServiceBackend.h"

namespace cx
{

SimulatedImageStreamerService::SimulatedImageStreamerService()
{
}

QString SimulatedImageStreamerService::getName()
{
	return "Simulator";
}

void SimulatedImageStreamerService::setImageToStream(QString imageUid)
{
	mImageUidToSimulate = imageUid;
	//Change image if streamer already exist
	if(!mStreamer)
	{
//		reporter()->sendInfo("SimulatedImageStreamerService::setImageToStream: No streamer yet. Image will be applied when streamer is created");
		return;
	}
	ImagePtr imageCopy = this->createImageCopy(imageUid);

	mStreamer->setSourceImage(imageCopy);
}

ImagePtr SimulatedImageStreamerService::createImageCopy(QString imageUid)
{
	cx::VideoServiceBackendPtr backend = cx::logicManager()->getVideoService()->getBackend();
	if(!backend)
	{
		reporter()->sendError("SimulatedImageStreamerService::createImageCopy: No VideoServiceBackend");
		return ImagePtr();
	}
	ImagePtr image = backend->getDataManager()->getImage(imageUid);
	if(!image)
	{
		reporter()->sendWarning("SimulatedImageStreamerService::createImageCopy: No image with uid: "+imageUid);
		return ImagePtr();
	}
	ImagePtr imageCopy = image->copy();
	return imageCopy;
}

void SimulatedImageStreamerService::setGain(double gain)
{
	QMutexLocker lock(&mStreamerMutex);
	if(mStreamer)
		mStreamer->setGain(gain);
}

StreamerPtr SimulatedImageStreamerService::createStreamer()
{
	QMutexLocker lock(&mStreamerMutex);
	mStreamer.reset(new SimulatedImageStreamer());
	//TODO: remove this dependency when TrackingManager are a plugin
	cx::VideoServiceBackendPtr backend = cx::logicManager()->getVideoService()->getBackend();
	if(!backend)
	{
		reporter()->sendError("SimulatedImageStreamerInterface got no VideoServiceBackend");
		return mStreamer;
	}

	ToolPtr tool = backend->getToolManager()->findFirstProbe();
	if(!tool)
		reporter()->sendWarning("No tool");

	ImagePtr imageCopy = this->createImageCopy(mImageUidToSimulate);
	mStreamer->initialize(imageCopy, tool, backend->getDataManager());

	return mStreamer;
}

QWidget *SimulatedImageStreamerService::createWidget()
{
	SimulateUSWidget* mSimulationWidget = new SimulateUSWidget(this);
	return mSimulationWidget;
}

} //end namespace cx
