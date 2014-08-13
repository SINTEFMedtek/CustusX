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

#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxVector3D.h"
#include "cxCyclicActionLogger.h"
#include "cxDirectlyLinkedSender.h"
#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxVideoServiceBackend.h"
#include "cxStreamerService.h"
#include "cxStreamer.h"
#include "cxDataLocations.h"
#include "cxXmlOptionItem.h"

namespace cx
{

DirectlyLinkedImageReceiverThread::DirectlyLinkedImageReceiverThread(StreamerServicePtr streamerInterface, QObject* parent) :
		ImageReceiverThread(parent), mStreamerInterface(streamerInterface)
{}

void DirectlyLinkedImageReceiverThread::run()
{
	report("Starting direct link grabber.");

	XmlOptionFile xmlFile = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("video");
	QDomElement element = xmlFile.getElement("video");
	mImageStreamer = mStreamerInterface->createStreamer(element);

	if(!mImageStreamer)
	{
		this->quit();
		std::cout << "quitting..." << std::endl;
		return;
	}
	mSender.reset(new DirectlyLinkedSender());

	connect(mSender.get(), SIGNAL(newImage()), this, SLOT(addImageToQueueSlot()), Qt::DirectConnection);
	connect(mSender.get(), SIGNAL(newUSStatus()), this, SLOT(addSonixStatusToQueueSlot()), Qt::DirectConnection);

	if(!mImageStreamer->startStreaming(mSender))
		this->quit();
	emit connected(true);

//	mFPSTimer->reset(2000);

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

} //end namespace cx

