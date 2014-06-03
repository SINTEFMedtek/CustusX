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
#include "cxImageStreamerFactory.h"

//#include "cxSimulatedImageStreamerInterface.h"
#include "cxCommandlineImageStreamerInterface.h"

namespace cx
{

ImageStreamerFactory::ImageStreamerFactory()
{
}

void ImageStreamerFactory::setBackend(VideoServiceBackendPtr backend)
{
	mBackend = backend;
}

void ImageStreamerFactory::setArguments(std::map<QString, QString> arguments)
{
	mArguments = arguments;
}

void ImageStreamerFactory::setImageToStream(QString imageUid)
{
	mImageUidToSimulate = imageUid;
}

//ImageStreamerInterfacePtr ImageStreamerFactory::getSimulatedStreamerInterface()
//{
//	SimulatedImageStreamerInterfacePtr streamerInterface(new SimulatedImageStreamerInterface());
//	streamerInterface->setBackend(mBackend);
//	streamerInterface->setImageToStream(mImageUidToSimulate);
//	return streamerInterface;
//}

StreamerServicePtr ImageStreamerFactory::getCommandlineStreamerInterface()
{
	CommandlineImageStreamerInterfacePtr streamerInterface(new CommandlineImageStreamerInterface());
	streamerInterface->setArguments(mArguments);
	return streamerInterface;
}

StreamerServicePtr ImageStreamerFactory::getStreamerInterface()
{
//	if(mArguments["type"] == "SimulatedImageStreamer")
//		return this->getSimulatedStreamerInterface();
//	else
		return this->getCommandlineStreamerInterface();
}

} //namespace cx
