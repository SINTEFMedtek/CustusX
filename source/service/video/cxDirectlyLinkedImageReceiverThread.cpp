/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxDirectlyLinkedImageReceiverThread.h"

#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxVector3D.h"
#include "cxCyclicActionLogger.h"
#include "cxDirectlyLinkedSender.h"
#include "cxTrackingService.h"
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

	XmlOptionFile xmlFile = XmlOptionFile(DataLocations::getXmlSettingsFile()).descend("video");
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

