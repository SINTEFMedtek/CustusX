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

#include "cxDirectlyLinkedImageReceiverThread.h"

#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscVector3D.h"
#include "cxImageSenderFactory.h"
#include "cxRenderTimer.h"
#include "cxDirectlyLinkedSender.h"

namespace cx
{

DirectlyLinkedImageReceiverThread::DirectlyLinkedImageReceiverThread(StringMap args, QObject* parent) :
		ImageReceiverThread(parent), mArguments(args)
{}

void DirectlyLinkedImageReceiverThread::run()
{
	ssc::messageManager()->sendInfo("Starting direct link grabber.");

	this->printArguments(); //debugging

//	if(magic_argument)
//		create_simulated_streamer()

	mImageStreamer = ImageStreamerFactory().getFromArguments(mArguments);
	if(!mImageStreamer)
	{
		this->quit();
		return;
	}
	mSender.reset(new DirectlyLinkedSender());

	connect(mSender.get(), SIGNAL(newImage()), this, SLOT(addImageToQueueSlot()), Qt::DirectConnection);
	connect(mSender.get(), SIGNAL(newUSStatus()), this, SLOT(addSonixStatusToQueueSlot()), Qt::DirectConnection);

	if(!mImageStreamer->startStreaming(mSender))
		this->quit();
	emit connected(true);

	mFPSTimer->reset(2000);

	// run event loop
	this->exec();

	mImageStreamer->stopStreaming();
	mImageStreamer.reset();
	mSender.reset();
	emit connected(false);
}

void DirectlyLinkedImageReceiverThread::addImageToQueueSlot()
{
	this->addImageToQueue(mSender->popImage());
}

void DirectlyLinkedImageReceiverThread::addSonixStatusToQueueSlot()
{
	this->addSonixStatusToQueue(mSender->popUSStatus());
}

QString DirectlyLinkedImageReceiverThread::hostDescription() const
{
	return "Direct Link";
}

void DirectlyLinkedImageReceiverThread::printArguments()
{
	ssc::messageManager()->sendDebug("-------------");
	ssc::messageManager()->sendDebug("DirectlyLinkedImageReceiverThread arguments:");
	StringMap::iterator it;
	for(it = mArguments.begin(); it != mArguments.end(); ++it)
	{
		ssc::messageManager()->sendDebug(it->first+": "+it->second);
	}
	ssc::messageManager()->sendDebug("-------------");
}

} //end namespace cx

