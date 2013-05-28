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

#include "cxGrabberReceiveThreadDirectLink.h"

#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscVector3D.h"
#include "cxImageSenderFactory.h"
#include "cxRenderTimer.h"
#include "cxGrabberSenderDirectLink.h"

namespace cx
{

GrabberDirectLinkThread::GrabberDirectLinkThread(StringMap args, QObject* parent) :
		GrabberReceiveThread(parent), mArguments(args)
{}

void GrabberDirectLinkThread::run()
{
	ssc::messageManager()->sendInfo("Starting direct link grabber.");

	mImageSender = ImageStreamerFactory().getFromArguments(mArguments);
	if(!mImageSender)
	{
		this->quit();
		return;
	}
	mGrabberBridge.reset(new GrabberSenderDirectLink());

	connect(mGrabberBridge.get(), SIGNAL(newImage()), this, SLOT(newImageSlot()), Qt::DirectConnection);
	connect(mGrabberBridge.get(), SIGNAL(newUSStatus()), this, SLOT(newUSStatusSlot()), Qt::DirectConnection);

	if(!mImageSender->startStreaming(mGrabberBridge))
		this->quit();
	emit connected(true);

	mFPSTimer->reset(2000);

	// run event loop
	this->exec();

	mImageSender->stopStreaming();
	mImageSender.reset();
	mGrabberBridge.reset();
	emit connected(false);
}

void GrabberDirectLinkThread::newImageSlot()
{
	this->addImageToQueue(mGrabberBridge->popImage());
}

void GrabberDirectLinkThread::newUSStatusSlot()
{
	this->addSonixStatusToQueue(mGrabberBridge->popUSStatus());
}

QString GrabberDirectLinkThread::hostDescription() const
{
	return "Direct Link";
}

} //end namespace cx

